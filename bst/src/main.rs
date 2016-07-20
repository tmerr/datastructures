type OBox<T> = Option<Box<T>>;

struct Node<K: Ord + Clone, D: Clone> {
    key: K,
    data: D,
    left: OBox<Node<K, D>>,
    right: OBox<Node<K, D>>
}

struct BST<K: Ord + Clone, D: Clone> {
    root: OBox<Node<K, D>>
}

impl<K: Ord + Clone, D: Clone> BST<K, D> {
    fn new() -> BST<K, D> {
        BST { root: None }
    }

    fn insert(&mut self, k: K, d: D) {
        fn helper<K2: Ord + Clone, D2: Clone>(n: &mut OBox<Node<K2, D2>>, k: K2, d: D2) {
            if let Some(ref mut boxed) = *n {
                let ref mut node = *boxed;
                if k < node.key {
                    helper(&mut node.left, k, d);
                } else {
                    helper(&mut node.right, k, d);
                }
            } else {
                *n = Some(Box::new(Node { key: k, data: d, left: None, right: None }));
            }
        }
        helper(&mut self.root, k, d);
    }

    fn search(&self, target: K) -> Option<D> {
        if let Some(ref boxed) = self.root {
            let mut cursor: &Node<K, D> = boxed;
            loop {
                if target == cursor.key {
                    return Some(cursor.data.clone());
                } else {
                    let side = if target < cursor.key {
                                   &cursor.left
                               } else {
                                   &cursor.right
                               };
                    if let Some(ref boxed) = *side {
                        cursor = boxed;
                    } else {
                        return None;
                    }
                }
            }
        } else {
            return None;
        }
    }

    fn minimum(&self) -> Option<(K, D)> {
        if let Some(ref boxed) = self.root {
            let mut cursor: &Node<K, D> = boxed;
            loop {
                if let Some(ref boxed) = cursor.left {
                    cursor = boxed;
                } else {
                    return Some((cursor.key.clone(), cursor.data.clone()));
                }
            }
        } else {
            return None;
        }
    }

    fn maximum(&self) -> Option<(K, D)> {
        if let Some(ref boxed) = self.root {
            let mut cursor: &Node<K, D> = boxed;
            loop {
                if let Some(ref boxed) = cursor.right {
                    cursor = boxed;
                } else {
                    return Some((cursor.key.clone(), cursor.data.clone()));
                }
            }
        } else {
            return None;
        }
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
