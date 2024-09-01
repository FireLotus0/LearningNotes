#include <iostream>
#include <string>
#include <memory>

class CarInterface{
public:
	CarInterface() =  default;
	~CarInterface() = default;

public:
	virtual void setEngine(const std::string& engine) = 0;
	virtual void setBrand(const std::string& brand) = 0;

	virtual std::string getEngine()  const  = 0;
	virtual std::string getBrand() const  = 0; 
};

class Car :  public CarInterface
{
public:
	void setEngine(const std::string& engine) {
		m_engine = engine;
	}

	void setBrand(const std::string& brand)  {
		m_brand = brand;
	}

	std::string getEngine() const { return m_engine; }

	std::string getBrand() const { return m_brand; }
	
private:
	std::string m_engine{};
	std::string m_brand{};

};

class CarBuilderInterface
{
public:
	virtual void setEngine(const std::string& engine) = 0;
	virtual void setBrand(const std::string& brand) = 0;

	virtual std::shared_ptr<Car> getCarInstance()  =  0;
};

class CarBuilder : public CarBuilderInterface
{
public:
	CarBuilder() {
		m_pCar = std::make_shared<Car>();
	}

	void setEngine(const std::string& engine)  override {
		m_pCar->setEngine(engine);
	}

	void setBrand(const std::string& brand)  override {
		m_pCar->setBrand(brand);
	}

	std::shared_ptr<Car> getCarInstance() override  { return m_pCar; }

private:
	std::shared_ptr<Car> m_pCar;
};

int main()
{
	CarBuilder carBuilder;

	carBuilder.setEngine("V8");
	carBuilder.setBrand("Benz");

	auto benz = carBuilder.getCarInstance();

	std::cout << "Engine: "  << benz->getEngine()
			  << " Brand: " << benz->getBrand() << std::endl;


	return 0;
}
