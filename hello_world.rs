use std::io::{Write,stdout};

fn print_letter(l: char) {
	print!("\x1b[1;32m{}\x1b[0m", l);
	stdout().flush().is_ok();
}

fn main() {
	print_letter('H');
	print_letter('e');
	print_letter('l');
	print_letter('l');
	print_letter('o');
	print_letter(' ');
	print_letter('D');
	print_letter('e');
	print_letter('v');
	print_letter(' ');
	print_letter('D');
	print_letter('a');
	print_letter('y');
	print_letter('s');
	print_letter(' ');
	print_letter('2');
	print_letter('0');
	print_letter('1');
	print_letter('8');
	print_letter('.');
	print_letter('\n');
	print_letter('D');
	print_letter('e');
	print_letter('b');
	print_letter('u');
	print_letter('g');
	print_letter('g');
	print_letter('i');
	print_letter('n');
	print_letter('g');
	print_letter(' ');
	print_letter('i');
	print_letter('s');
	print_letter(' ');
	print_letter('n');
	print_letter('o');
	print_letter('t');
	print_letter(' ');
	print_letter('t');
	print_letter('h');
	print_letter('a');
	print_letter('t');
	print_letter(' ');
	print_letter('h');
	print_letter('a');
	print_letter('r');
	print_letter('d');
	print_letter('.');
	print_letter('.');
	print_letter('.');
	print_letter('\n');
}
