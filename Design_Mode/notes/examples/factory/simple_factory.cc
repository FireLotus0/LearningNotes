#include <iostream>
#include <string>
#include <vector>

class ProductInterface;

class FactoryInterface{
public:
	FactoryInterface() = default;
	~FactoryInterface() {}

	virtual ProductInterface* produce(const std::string& condition) = 0;
};


class ProductInterface{
public:
	ProductInterface() = default;
	~ProductInterface(){}

	virtual std::string getProductName() const = 0;

};


class ProductApple : public ProductInterface
{
public:
	
	std::string getProductName() const override {
		return "apple";
	} 
};

class ProductPear :  public ProductInterface
{
public:
	std::string getProductName() const override {
		return "Pear";
	}
};


class FactoryImpl :  public FactoryInterface
{
public:
	ProductInterface* produce(const std::string& condition) override{
		if(condition ==  "apple") {
			return new ProductApple{};
		} else if(condition == "pear")  {
			return new ProductPear{};
		} else {
			return nullptr;
		}
	}
};

int main()
{
	FactoryImpl factory;

	std::vector<ProductInterface*> products;
	for(int i = 0;  i  <  4; i++) {
		products.push_back(factory.produce(i % 2 == 0 ? "apple" : "pear"));
	}

	for(auto p : products)  {
		std::cout << "product name: " << p->getProductName() << std::endl;
	}

	return 0;
}
