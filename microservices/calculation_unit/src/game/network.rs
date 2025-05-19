use std::{cell::RefCell, rc::Rc};

type NodeWrapper<T> = Rc<RefCell<NetworkNode<T>>>;

pub struct NetworkNode<T> {
	pub value: T,
	pub connections: Vec<NodeWrapper<T>>,
}

impl<T> NetworkNode<T> {
	// base constructor with no connections
	fn new(value: T) -> Self {
		let node = Self {
			value,
			connections: vec![],
		};
		node
	}

	// base constructor with no connections
	pub fn wrapper(value: T) -> NodeWrapper<T> {
		let node = Self::new(value);
		let wrapper = Rc::new(RefCell::new(node));
		wrapper
	}
}

// connect a nodewrapper oneway with another
pub fn connect<T>(a: &NodeWrapper<T>, b: &NodeWrapper<T>) {
	a.borrow_mut().connections.push(Rc::clone(b));
}

// connect a nodewrapper twoway with another
pub fn connect_twoway<T>(a: &NodeWrapper<T>, b: &NodeWrapper<T>) {
	a.borrow_mut().connections.push(Rc::clone(b));
	b.borrow_mut().connections.push(Rc::clone(a));
}

#[cfg(test)]
pub mod network_tests {
	use super::{NetworkNode, NodeWrapper, connect, connect_twoway};

	fn print_value(node: &NodeWrapper<i32>) {
		let value = node.borrow().value;
		println!("node value is {}", value);
	}
	fn print_connections(node: &NodeWrapper<i32>) {
		let values = &node.borrow().connections;
		for value in values.iter() {
			print_value(value);
		}
	}

	#[test]
	fn test_base_network() {
		let one = NetworkNode::wrapper(1);
		let two = NetworkNode::wrapper(2);
		let three = NetworkNode::wrapper(3);

		connect(&one, &two);
		connect_twoway(&two, &three);

		println!("one connections: ");
		print_connections(&one);
		println!("two connections: ");
		print_connections(&two);
		println!("three connections: ");
		print_connections(&three);

		assert_eq!(one.borrow().connections.len(), 1);
		assert_eq!(two.borrow().connections.len(), 1);
		assert_eq!(three.borrow().connections.len(), 1);
	}
}
