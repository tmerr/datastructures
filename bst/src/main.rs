use std::cmp::Ordering;

enum BST<K: Ord + Clone, D: Clone> {
    Empty,
    Full(K, D, Box<BST<K, D>>, Box<BST<K, D>>)
}

impl<K: Ord + Clone, D: Clone> BST<K, D> {
    fn new() -> BST<K, D> {
        BST::Empty
    }

    fn insert(&mut self, k: K, d: D) {
        if let BST::Full(ref key, _, ref mut left, ref mut right) = *self {
            if k < *key {
                left.insert(k, d);
            } else {
                right.insert(k, d);
            }
        } else {
            *self = BST::Full(k, d, Box::new(BST::Empty), Box::new(BST::Empty));
        }
    }

    fn search(&self, k: K) -> Option<D> {
        if let BST::Full(ref key, ref data, ref left, ref right) = *self {
            match k.cmp(&key) {
                Ordering::Equal => Some(data.clone()),
                Ordering::Less => (*left).search(k),
                Ordering::Greater => (*right).search(k)
            }
        } else {
            None
        }
    }

    fn minimum(&self) -> Option<(K, D)> {
        let mut node = self;
        while let BST::Full(ref key, ref data, ref left, _) = *node {
            if let BST::Empty = **left {
                return Some((key.clone(), data.clone()));
            }
            node = left;
        }
        return None;
    }

    fn maximum(&self) -> Option<(K, D)> {
        let mut node = self;
        while let BST::Full(ref key, ref data, _, ref right) = *node {
            if let BST::Empty = **right {
                return Some((key.clone(), data.clone()));
            }
            node = right;
        }
        return None;
    }
}

fn main() {
    println!("Hello, world!");
}

#[cfg(test)]
mod tests {
    use super::BST;

    #[test]
    fn test_insert10() {
        let data = [3, 7, 2, 3, 7, 1, 4, 7, 11, 6];
        let mut bst = BST::new();
        for &i in data.iter() {
            bst.insert(i, i);
        }
        if let Some((i, _)) = bst.minimum() {
            assert!(i == 1, "minimum failed: wrong value");
        } else {
            panic!("minimum failed: no minimum");
        }
        if let Some((i, _)) = bst.maximum() {
            assert!(i == 11, "maximum failed: wrong value");
        } else {
            panic!("maximum failed: no maximum");
        }

        for &i in data.iter() {
            assert!(bst.search(i).is_some());
        }
        assert!(bst.search(69).is_none());
    }

    #[test]
    fn test_empty() {
        let bst: BST<u8, u8> = BST::new();
        assert!(bst.minimum().is_none());
        assert!(bst.maximum().is_none());
        assert!(bst.search(5).is_none());
    }
}
