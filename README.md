# Elegant Abstract Factory

A C++26 Reflection-based header-only Generic Abstract Factory library. 

Often times the Abstract Factory pattern requires a large if-else or switch chain to convert from a string or enum type to an abstract type. This adds an extra step to extending a heirarchy horizontally. The Elegant Abstract Factory changes that: By automatically generating the factory at compile time, you no longer need to add these conditionals or look at a very long chain of if statements. A key limitation is that the arguments of each constructor registered to the factory must be the same. 

## How it works: 

Automatically generates a factory make function to create pointers to a derived class. Can create an object just based off the string class name.  All classes "registered" to the factory are registered at compile time using annotations. 

## Example
```

struct Register; // Type to "tag" classes to be registered
Register r;

class Base { 
    // ...
};

[[=r]] // Annotated derived class
class Derived1 : public Base {
 // ...
};

[[=r]]
class DerivedN  : public Base {
 // ...
};

eaf::AbstractFactory<Register, Base> factory;
std::unique_ptr<Base> derived = factory.make_shared("Derived1");
```

## Roadmap
- Register classes from multiple namespaces
    - Could an array template argument to make_shared work? Variadic param is reserved for constructor args 
- Improve CMake further so that the project can be absorbed via fetch_content
- Register mappings from type T to Class name to constuct from an identifier