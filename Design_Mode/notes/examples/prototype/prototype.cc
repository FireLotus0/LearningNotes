#include <iostream>
#include <string>
#include <memory>

class AbstractJobSeeker
{
public:
	AbstractJobSeeker() = default;
	AbstractJobSeeker(const std::string& name,  const std::string& skill)
		:  m_name(name)
		, m_skill(skill)  
		{}

	~AbstractJobSeeker() {}

public:
	virtual void setName(const std::string& name) = 0;
	virtual void setSkill(const std::string& skill) =  0;

	virtual void show() const = 0;

	virtual std::unique_ptr<AbstractJobSeeker> clone() = 0;
protected:
	std::string m_name{};
	std::string m_skill{};
};


class JobSeeker : public AbstractJobSeeker
{
public:
	JobSeeker()  = default;
	JobSeeker(const std::string name, const std::string& skill)
		:  AbstractJobSeeker(name, skill)
		{}

	JobSeeker(const JobSeeker& other) {
		std::cout << "copy constructor called!" << std::endl;
		m_name = other.m_name;
		m_skill = other.m_skill;
	}

public:
	void setName(const std::string& name) override { m_name  = name; }
	void setSkill(const std::string& skill)  override { m_skill = skill; }

	void show() const  override { std::cout << "job seeker:  name--" << m_name << " skill--"<< m_skill << std::endl; }

	std::unique_ptr<AbstractJobSeeker> clone() override { return std::make_unique<JobSeeker>(*this); }
};

int main()
{
	JobSeeker seekerA{"jack", "熟悉c++"};
	auto seekerB = seekerA.clone();
	seekerB->setName("Bob");
	seekerB->setSkill("熟悉Python");

	seekerA.show();
	seekerB->show();

	return 0;
}
