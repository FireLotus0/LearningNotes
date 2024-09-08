#include <string>
#include <iostream>
#include <memory>

class ComponentInterface
{
public:
	virtual void operation() = 0;

	~ComponentInterface() {}
};

class ConcreteComponent : public ComponentInterface
{
public:
	ConcreteComponent() = default;

	void operation() override {
		std::cout << "ConcreteComponent operation..." << std::endl;
	}
};

class Decorator : public ComponentInterface
{
public:
	void setComponent(const std::shared_ptr<ConcreteComponent>& component) {
		m_component = component;
	}

	void operation() override {
		if(m_component != nullptr) {
			m_component->operation();
		}
	}
	
private:
	std::shared_ptr<ConcreteComponent> m_component;
};

class DecoratorA : public Decorator
{
public:
	void operation() {
		Decorator::operation();
		std::cout << "decorator in DecoratorA" << std::endl;
	}
};

class DecoratorB : public Decorator
{
public:
	void operation() {
		Decorator::operation();
		std::cout << "operation in DecoratorB" << std::endl;
	}
};


int main() 
{
	auto component = std::make_shared<ConcreteComponent>();

	auto decoratorA = std::make_shared<DecoratorA>();
	decoratorA->setComponent(component);

	auto decoratorB = std::make_shared<DecoratorB>();
	decoratorB->setComponent(component);

	decoratorA->operation();
	decoratorB->operation();

	return 0;
}



