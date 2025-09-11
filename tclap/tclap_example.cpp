#include <algorithm>
#include <iostream>
#include <string>
#include <iomanip>

#include "tclap/CmdLine.h"

using namespace TCLAP;
using namespace std;

int main(int argc, char **argv) {
  try {
    /**
     * CmdLine(const std::string& message, const char delimiter = ' ', const std::string& version = "none", bool helpAndVersion = true);
     * @brief constructs a CmdLine object that will parse the command line arguments
     * @param message The message will be used in the usage statement.
     * @param delimiter 分隔符，用于分割命令行参数的，默认是“ ”
     * @param version 版本号，用于--version选项
     * @param helpAndVersion 是否自动添加--help和--version选项，默认是true
     */
    CmdLine cmd("Command description message", ' ', "0.9");

    /**
     * template<class T> class ValueArg : public Arg
     * @note 永远不要直接构造一个 Arg 对象。Arg 是一个抽象基类，不能被实例化。
     * @brief 用于定义一个带有值的命令行参数，例如 --name Homer
     * 
     * ValueArg( const std::string& flag, const std::string& name, const std::string& desc, bool req, T value, const std::string& typeDesc, Visitor* v = NULL);
     * @param flag 一个字符，用于表示参数，例如 -n
     * @param name 一个单词，参数的名称，例如 --name
     * @param desc 参数的描述
     * @param req 参数是否是必须的
     * @param value 参数的默认值，当命令行没有提供该参数时使用
     * @param typeDesc 参数的类型描述，例如 "string"
     * @param v 一个可选的访问者对象，用于在参数被设置时执行特定的操作
     * 
     */
    ValueArg<string> nameArg("n", "name", "Name to print", true, "homer", "string");
    cmd.add(nameArg);
    ValueArg<int> ageArg("a", "age", "Age to print", false, 30, "int");
    cmd.add(ageArg);

    /**
     * class SwitchArg : public Arg
     * @note 永远不要直接构造一个 Arg 对象。Arg 是一个抽象基类，不能被实例化。
     * @brief 用于定义一个开关参数（布尔值），例如 --reverse
     * 
     * SwitchArg(const std::string& flag, const std::string& name, const std::string& desc, bool def = false, Visitor* v = NULL);
     * @param flag 一个字符，用于表示参数，例如 -r
     * @param name 一个单词，参数的名称，例如 --reverse
     * @param desc 参数的描述
     * @param def 参数的默认值，当命令行没有提供该参数时使用
     * @param v 一个可选的访问者对象，用于在参数被设置时执行特定的操作
     * 
     * @note 此时getValue()返回的是bool类型, true表示开关被设置，false表示未设置
     */
    SwitchArg reverseSwitch("r", "reverse", "Print name backwards", false);
    cmd.add(reverseSwitch);

    cmd.parse(argc, argv);

    string name = nameArg.getValue();
    std::string name_flag = nameArg.getFlag();
    std::cout << "name flag: " << name_flag << std::endl;
    std::string name_name = nameArg.getName();
    std::cout << "name name: " << name_name << std::endl;
    bool name_required = nameArg.isRequired();
    std::cout << "name required: " << std::boolalpha << name_required << std::endl;
    bool name_set = nameArg.isSet();
    std::cout << "name set: " << std::boolalpha << name_set << std::endl;
    bool name_ignore = nameArg.isIgnoreable();
    std::cout << "name ignore: " << std::boolalpha << name_ignore << std::endl;
    std::string name_tostring = nameArg.toString();
    std::cout << "name toString: " << name_tostring << std::endl;

    bool age_set = ageArg.isSet();
    if (age_set) {
      int age = ageArg.getValue();
      std::cout << "Age: " << age << std::endl;
    } else {
      std::cout << "Age not set." << std::endl;
    }

    bool reverseName = reverseSwitch.getValue();

    std::cout << "===============================" << std::endl;
    if (reverseName) {
      reverse(name.begin(), name.end());
      cout << "My name (spelled backwards) is: " << name << endl;
    } else
      cout << "My name is: " << name << endl;

  } catch (ArgException &e) // catch any exceptions
  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }
}
