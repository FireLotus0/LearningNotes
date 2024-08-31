#include <iostream>
#include <vector>
#include <string>

enum class FactoryType
{
	Fruit,
	Car
};

class ProductInterface {
public:
	ProductInterface() = default;
	~ProductInterface() {}

	virtual std::string  getProductName() const  = 0;
};

class ProductFruitApple : public  ProductInterface
{
public:
	std::string getProductName() const override
	{
		return "apple";
	}
};

class ProductFruitPear : public ProductInterface
{
public:
	std::string getProductName() const override {
		return "pear";
	}
};

class ProductCarBenz : public ProductInterface
{
public:
	std::string getProductName() const override {
		return "Benz";
	}
};

class ProductCarPosche : public ProductInterface
{
public:
	std::string getProductName() const override {
		return "posche";
	}
};

class FactoryInterface
{
public:
	FactoryInterface() = default;
	~FactoryInterface() {}

	virtual ProductInterface* produce(const std::string& condition) =  0;
	virtual FactoryType getFactoryType() const =  0;
};

class FactoryFruit : public FactoryInterface
{
public:
	ProductInterface* produce(const std::string& condition) override {
		if(condition == "apple") {
			return new ProductFruitApple;
		} else if(condition == "pear"){
			return new  ProductFruitPear;
		} else {
			return nullptr;
		}
	}

	FactoryType getFactoryType() const override { return FactoryType::Fruit; }
};

class FactoryCar : public FactoryInterface
{
public:
	ProductInterface* produce(const std::string& condition) override {
		if(condition == "benz") {
			return new ProductCarBenz;
		} else if(condition == "posche" ) {
			return new ProductCarPosche;
		} else {
			return nullptr;
		}
	}

	FactoryType getFactoryType() const override { return FactoryType::Car; }
};




class AbstractFactory
{
public:
	AbstractFactory() = default;
	~AbstractFactory() {}

	FactoryInterface* getFactory(FactoryType factoryType) {
		switch((int)factoryType) {
			case (int)FactoryType::Fruit:  return new FactoryFruit;
			case (int)FactoryType::Car: return new FactoryCar;
			default: return nullptr;
		}
	}
};

int main()
{
	AbstractFactory abstractFactory;

	std::vector<FactoryInterface*> factories;
	factories.push_back(abstractFactory.getFactory(FactoryType::Fruit));
	factories.push_back(abstractFactory.getFactory(FactoryType::Car));

	std::vector<ProductInterface*> products;

	for(int i = 0; i  < 10; i++) {
		auto factory = factories[i % 2];
		if(factory->getFactoryType() ==  FactoryType::Fruit) {
			products.push_back(factory->produce(i % 2 ==  0 ? "apple" : "pear"));
		} else {
			products.push_back(factory->produce(i % 2 ==  0 ? "benz" :  "posche"));
		}
	}


	for(auto productPtr : products)  {
		std::cout << "product name is: " << productPtr->getProductName() << std::endl;
	}


}
