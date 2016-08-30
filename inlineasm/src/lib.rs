#![feature(test)]
#![feature(asm)]

extern crate test;

use std::ops::ShrAssign;


fn shift_all<T: ShrAssign<T> + Copy>(data: &mut[T], shft: T) {
    for d in data {
        *d >>= shft;
    }
}

/// Does nothing but move around a pointer across the slice.
#[cfg(any(target_arch = "x86_64"))]
#[no_mangle]
pub fn point_asm(data: &mut [u64]) {
    unsafe {
        let ptr = data.as_ptr();
        let mut i: u64 = 0;
        asm!(r#"movq $1, $0
                addq $2, $0
                1:
                subq $$8, $0
                cmpq $0, $1
                jne 1b"#
             : "=&r"(i)
             : "r"(ptr), "r"(8*data.len() - 8)
             : "memory", "cc"
            );
    }
}

/// Does nothing but read each u64 into a temporary register.
#[cfg(any(target_arch = "x86_64"))]
#[no_mangle]
pub fn read_asm(data: &mut [u64]) {
    unsafe {
        let ptr = data.as_ptr();
        let mut i: u64 = 0;
        let mut tmp: u64 = 0;
        asm!(r#"movq $2, $0
                addq $3, $0
                1:
                subq $$8, $0
                movq ($0), $1
                cmpq $0, $2
                jne 1b"#
             : "=&r"(i), "=&r"(tmp)
             : "r"(ptr), "r"(8*data.len())
             : "memory", "cc"
            );
    }
}

/// Does nothing but read each u64 into a temporary register then write it back out.
#[cfg(any(target_arch = "x86_64"))]
#[no_mangle]
pub fn read_write_asm(data: &mut [u64]) {
    unsafe {
        let ptr = data.as_ptr();
        let mut i: u64 = 0;
        let mut tmp: u64 = 0;
        asm!(r#"movq $2, $0
                addq $3, $0
                1:
                subq $$8, $0
                movq ($0), $1
                movq $1, ($0)
                cmpq $0, $2
                jne 1b"#
             : "=&r"(i), "=&r"(tmp)
             : "r"(ptr), "r"(8*data.len())
             : "memory", "cc"
            );
    }
}

/// Does nothing but read each u64 into a temporary register then write it back out,
/// using an unrolled loop.
#[cfg(any(target_arch = "x86_64"))]
#[no_mangle]
pub fn read_write_asm_unrolled(data: &mut [u64]) {
    // Unroll the loop to try to get the CPU to parallelize instructions.
    // Didn't make any difference on my Core 2 era Xeon.
    unsafe {
        let ptr = data.as_ptr();
        let mut i: u64 = 0;
        let mut t1: u64 = 0;
        let mut t2: u64 = 0;
        let mut t3: u64 = 0;
        let mut t4: u64 = 0;
        asm!(r#"movq $5, $0
                addq $6, $0

                1:
                subq $$32, $0

                movq ($0), $1
                movq 8($0), $2
                movq 16($0), $3
                movq 24($0), $4

                movq $1, ($0)
                movq $2, 8($0)
                movq $3, 16($0)
                movq $4, 24($0)

                cmpq $0, $5
                jne 1b"#
             : "=&r"(i), "=&r"(t1), "=&r"(t2), "=&r"(t3), "=&r"(t4)
             : "r"(ptr), "r"(8*data.len())
             : "memory", "cc"
            );
    }
}

/// Performs an in place right bitshift on an arbitrarily sized &mut [u64].
/// The shift amount must be at most 31.
#[cfg(any(target_arch = "x86_64"))]
#[no_mangle]
pub fn shift_asm(data: &mut [u64], shift: u8) {
    unsafe {
        let ptr = data.as_ptr();
        let mut i: u64 = 0;
        let mut tmp: u64 = 0;
        asm!(r#"movq $2, $0
                addq $4, $0
                1:
                subq $$8, $0
                movq ($0), $1
                shrd $1, 8($0)
                cmpq $2, $0
                jne 1b
                movq $$0, $1
                shrd $1, ($2)"#
            : "=&r"(i), "=&r"(tmp)
            : "r"(ptr), "{cl}"(shift), "r"(8*data.len() - 8)
            : "memory", "cc"
            );
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;

    const test_bytes: usize = 100_000_000;

    #[test]
    fn test_shift_asm() {
        let input = 0x10000000_00000001_u64;
        let expected_first = 0x00200000_00000000_u64;
        let expected_rest = 0x02200000_00000000_u64;
        let mut v = vec![input; test_bytes/8];
        super::shift_asm(&mut v[..], 7);
        assert_eq!(v[0], expected_first);
        assert_eq!(v[1], expected_rest);
        assert_eq!(v[100], expected_rest);
        assert_eq!(v[1000], expected_rest);
        assert_eq!(v[test_bytes/8-1], expected_rest);
    }

    #[bench]
    fn bench_point_asm(b: &mut Bencher) {
        let input = 0x10000000_00000001_u64;
        let mut v = vec![input; test_bytes/8];
        b.iter(|| super::point_asm(&mut v[..]));
    }

    #[bench]
    fn bench_read_asm(b: &mut Bencher) {
        let input = 0x10000000_00000001_u64;
        let mut v = vec![input; test_bytes/8];
        b.iter(|| super::read_asm(&mut v[..]));
    }

    #[bench]
    fn bench_read_write_asm(b: &mut Bencher) {
        let input = 0x10000000_00000001_u64;
        let mut v = vec![input; test_bytes/8];
        b.iter(|| super::read_write_asm(&mut v[..]));
    }
    #[bench]

    fn bench_read_write_asm_unrolled(b: &mut Bencher) {
        let input = 0x10000000_00000001_u64;
        let mut v = vec![input; test_bytes/8];
        b.iter(|| super::read_write_asm_unrolled(&mut v[..]));
    }

    #[bench]
    fn bench_shift_asm(b: &mut Bencher) {
        let input = 0x10000000_00000001_u64;
        let mut v = vec![input; test_bytes/8];
        b.iter(|| super::shift_asm(&mut v[..], 7));
    }

    #[bench]
    fn bench_u8_shift(b: &mut Bencher) {
        let mut data = vec![0xCC_u8; test_bytes];
        b.iter(|| super::shift_all(&mut data, 7));
    }

    #[bench]
    fn bench_u16_shift(b: &mut Bencher) {
        let mut data = vec![0xCC_CC_u16; test_bytes/2];
        b.iter(|| super::shift_all(&mut data, 7));
    }

    #[bench]
    fn bench_u32_shift(b: &mut Bencher) {
        let mut data = vec![0xCC_CC_CC_CC_u32; test_bytes/4];
        b.iter(|| super::shift_all(&mut data, 7));
    }

    #[bench]
    fn bench_u64_shift(b: &mut Bencher) {
        let mut data = vec![0xCC_CC_CC_CC_CC_CC_CC_CC_u64; test_bytes/8];
        b.iter(|| super::shift_all(&mut data, 7));
    }

    #[bench]
    fn bench_usize_shift(b: &mut Bencher) {
        let mut data = vec![0xCC_CC_CC_CC_CC_CC_CC_CC_usize; test_bytes/8]; // 8 for 64-bit
        b.iter(|| super::shift_all(&mut data, 7));
    }

}
