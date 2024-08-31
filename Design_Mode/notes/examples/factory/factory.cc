#include <iostream>
#include <string>
#include <vector>

class ProductInterface;
class ProductApple;
class ProductPear;

class FactoryInterface {
public:
	FactoryInterface()  = default;
	~FactoryInterface() {}

	virtual ProductInterface* produce() = 0;
};

class FactoryApple : public FactoryInterface
{
public:
	ProductInterface* produce() override {
		return new ProductApple;
	}
};

class FactoryPear : public FactoryInterface
{
public:
	ProductInterface* produce() override {
		return new ProductPear;
	}
};

class ProductInterface
{
public:
	ProductInterface() = default;
	~ProductInterface() {}

	virtual std::string getProductName() const = 0;
};

class  ProductApple : public ProductInterface
{
public:
	std::string getProductName() const override {
		return "apple";
	}
};

class ProductPear : public ProductInterface
{
public:
	std::striing getProductName() const  override  {
		return "pear";
	}
};


int main()
{
	std::vector<FactoryInterface*> factories {new FactoryApple,  new FactoryPeaar};
	std::vector<ProductInterface*> products;

	for(int i = 0; i <  4;  i++)  {
		products.push_back(factories[i %  2]->produce());
	}

	for(auto p :  products)  {
		std::cout << "product name:  "<<p->getProductName() << std::endl;
	}

	return 0;
}

