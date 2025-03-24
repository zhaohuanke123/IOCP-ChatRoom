#include <net_lib/json.hpp> // 需要安装 nlohmann/json 库
#include <iostream>

using json = nlohmann::json;

// 定义一个简单的结构体
struct Person {
    std::string name;
    int age;

    // 序列化方法
    json to_json() const {
        return json{{"name", name}, {"age", age}};
    }

    // 反序列化方法
    static Person from_json(const json& j) {
        Person p;
        j.at("name").get_to(p.name);
        j.at("age").get_to(p.age);
        return p;
    }
};

int main() {
    // 创建一个对象
    Person person{"Alice", 25};

    // 序列化到JSON
    json serialized = person.to_json();
    std::cout << "Serialized JSON: " << serialized.dump() << std::endl;

    // 从JSON反序列化
    Person deserialized = Person::from_json(serialized);
    std::cout << "Deserialized Person: " << deserialized.name << ", " << deserialized.age << std::endl;

    return 0;
}
