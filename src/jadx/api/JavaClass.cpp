/* Copyright 2018 R. Thomas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <jadx/api/JavaClass.hpp>

#include <java/util/List.hpp>

#include <fstream>

namespace jni::jadx::api {

JavaClass::JavaClass(JNIEnv& env, const Object_t& obj) {
  this->env_ = &env;
  this->obj_ = NewLocal(env, obj);
}

JavaClass::JavaClass(JNIEnv& env, const Object<>& obj) {

  this->env_ = &env;
  auto&& casted = Cast<JavaClassTag>(this->env(), this->clazz(), obj);
  this->obj_ = NewLocal(env, casted);
}

std::string JavaClass::getCode() {
  try {
    static auto&& getCode = this->clazz().GetMethod<String()>(this->env(), "getCode");
    return Make<std::string>(this->env(), this->obj_.Call(this->env(), getCode));
  } catch (const jni::PendingJavaException& e) {
    return "";
  }
}

std::string JavaClass::getSmali() {
  try {
    static auto&& getCode = this->clazz().GetMethod<String()>(this->env(), "getSmali");
    return Make<std::string>(this->env(), this->obj_.Call(this->env(), getCode));
  } catch (const jni::PendingJavaException& e) {
    return "";
  }
}


bool JavaClass::save(const std::string& path) {
  std::ofstream output{path, std::ios::trunc};
  if (not output) {
    std::cerr << "Can't save to '" << path << "'" << std::endl;
    return false;
  }
  output << this->getCode();
  return true;
}


bool JavaClass::save_smali(const std::string& path) {
  std::ofstream output{path, std::ios::trunc};
  if (not output) {
    std::cerr << "Can't save to '" << path << "'" << std::endl;
    return false;
  }
  output << this->getSmali();
  return true;
}


JavaClass::methods_list_t JavaClass::methods(void) {
  using List_t = java::util::List<JavaMethod>;
  static auto&& getMethods = this->clazz().template GetMethod<Object<List_t::Tag>()>(this->env(), "getMethods");

  auto&& jmethods = this->obj_.Call(this->env(), getMethods);

  List_t jlist{this->env(), jmethods};

  JavaClass::methods_list_t methods;
  methods.reserve(jlist.size());

  for (size_t i = 0; i < jlist.size(); ++i) {
    methods.emplace_back(jlist.at(i));
  }

  return methods;
}

}
