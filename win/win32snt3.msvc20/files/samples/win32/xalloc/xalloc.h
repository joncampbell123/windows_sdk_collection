/* Create a user-defined exception derived from the 'range' standard
exception */
class  NonPositive: public range
{
public:
	NonPositive(const __exString& what): range(what) {};
	void raise()	{ handle_raise();	throw *this;	};
};

