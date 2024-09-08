#include <iostream>
#include <memory>
#include <string>

class ProxyInterface
{
public:
	virtual ~ProxyInterface() {}

	virtual void request(const std::string&, const std::string&) = 0;
};

class RealServer : public ProxyInterface
{
public:
	void request(const std::string&, const std::string&) override {
		std::cout << "访问服务器资源..." << std::endl;
	}
};

class ProxyServer : public ProxyInterface
{
public:
	ProxyServer(const std::string& name, const std::string& passwd)
		: ProxyInterface()
		, m_name(name)
		, m_passwd(passwd)
		, m_server(std::make_shared<RealServer>())
		{}

		void request(const std::string& name, const std::string& passwd) override {
			if(!loginCheck(name, passwd)) {
				std::cout << "Error: 用户名，密码校验失败..." << std::endl;
				return;
			}
			beforeRequest();

			m_server->request(m_name, m_passwd);

			afterRequest();
		}

private:
	bool loginCheck(const std::string& name, const std::string& passwd) {
		return m_name == name && m_passwd == passwd;
	}

	void beforeRequest() {
		std::cout << "访问服务器资源之前..." << std::endl;
	}

	void afterRequest() {
		std::cout << "访问服务器资源之后..." << std::endl;
	}

private:
	std::string m_name;
	std::string m_passwd;
	std::shared_ptr<RealServer> m_server;
};


int main()
{
	auto serverProxy = std::make_shared<ProxyServer>("AAA", "123");

	serverProxy->request("AAA", "111");
	serverProxy->request("AAA", "123");
}
