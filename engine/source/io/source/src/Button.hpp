#ifndef _BUTTON_HPP

#define _BUTTON_HPP

#define JUST_PRESSED	0
#define PRESSED			1
#define RELEASED		2
#define JUST_RELEASED	3
#define REPEAT			4
/* Button: part of the input system, it represents
 * the state of a button mapped to a specific key
 */
//TODO: Define the equality operation
struct Button {
	Button() = default;
	explicit Button(const int bound_key) : m_state(RELEASED), m_bound_key(bound_key) {};
	
	bool operator<(const Button & rhs);
	
	void update(const int state);

	int		m_state = RELEASED;
	int		m_bound_key;
};

bool operator<(const Button &lhs, const Button &rhs);

#endif // !_BUTTON_HPP
