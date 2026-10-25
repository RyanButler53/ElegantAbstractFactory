# Elegant Abstract Factory

A C++26 Reflection-based header-only Generic Abstract Factory library. 

Often times the Abstract Factory pattern requires a large if-else or switch chain to convert from a string or enum type to an abstract type. This adds an extra step to extending a heirarchy horizontally. The Elegant Abstract Factory changes that: By automatically generating the factory at compile time, you no longer need to add these conditionals or write a long switch: The compiler automatically adds these to the abstract factory. A key limitation is that the types of arguments of each constructor registered to the factory must be the same. 

## How it works: 

Automatically generates a factory make function to create pointers to a derived class. Can create an object just based off the string class name.  All classes "registered" to the factory are registered at compile time using annotations.  

## Example
```

struct Register; // Type to "tag" classes to be registered
Register r;

class Base { 
    // ...
};

 // Annotated derived class
class [[=r]] Derived1 : public Base {
 // ...
};

class DerivedN [[=r, eaf::String("derived class"), =32]]
 : public Base {
 // ...
};

eaf::AbstractFactory<Register, Base, ^^::, eaf::String> factory;
std::unique_ptr<Base> derived = factory.make_unique("Derived1");
std::shared_ptr<Base> stringTag = factory.make_shared(eaf::String("derived class"));
std::unique_ptr intTag = factory.make_unique(32); // CTAD works here too
```

## Providing alternative mappings to generate classes: 
Sometimes the entire class name is long or the class is set to be constructed from a another type (typically a string, integral type or enum).
The last template argument of `eaf::AbstractFactory` is `Mapping`, which is the type of an alternative way to construct the object.  To enable an alternative
way of constructing the object, add an additional annotation in the class definition to provide the alternative mapping. Since strings have dynamic storage allocation, you must use eaf::String, which allows a string to be used and mapped to for reflection annotations. There can be multiple different annotated mappings for a class, but a factory with mapping type `Mapping` can only construct from annotations with type `Mapping`. Two different factories can produce the same class if the class is annotated with two types. (See customMappings.cpp unit test)


## Roadmap
- Register classes from multiple namespaces
    - Could an array template argument to make_shared work? Variadic param is reserved for constructor args 
- Improve CMake further so that the project can be absorbed via fetch_content
