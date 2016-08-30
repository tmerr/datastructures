Bitshifts don't matter
====================

Which is faster in Rust?
- Bitshifting each u8 in a 100MB array
- Bitshifting each u16 in a 100MB array
- Bitshifting each u32 in a 100MB array
- Bitshifting each u64 in a 100MB array
- Bitshifting each u64 in a 100MB array using inline assembly

For my system the answer is that every single one takes 52 milliseconds. The reason is that the relative cost of bitshifting
compared to memory access is so outrageously low, it doesn't matter how you do it. In one final test I tried something silly but
enlightening: iterate through the array and do nothing but read each u64 into a register then write it back
out unmodified. This once again took 52 milliseconds, just like when bitshifting.

On a side note here is how to disassemble a Rust library. This can come in handy when debugging [inline](https://doc.rust-lang.org/book/inline-assembly.html) [assembly](http://llvm.org/docs/LangRef.html#inline-assembler-expressions):
- cargo build to make the `.rlib` file
- `objdump -d thelibrary.rlib` to get the textual assembly output
