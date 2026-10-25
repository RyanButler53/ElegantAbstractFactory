# Elegant Abstract Factory

A C++26 Reflection-based header-only Generic Abstract Factory library. 

The Abstract Factory pattern often requires a large if-else or switch chain to convert from a string or enum type to an pointer to an abstract interface. This adds an extra step to extending a heirarchy horizontally. The Elegant Abstract Factory bypasses this: By automatically using class type annotations and static reflection, the factory method can be generated at compile time. A key limitation is that the types of arguments of each constructor registered to the factory must be the same. 

## How it works: 

The `eaf::AbstractFactory::make` function looks through the provided namespace (via `members_of`) and filters for reflections that meet 4 conditions:
    - The reflected entity is a complete type
    - The reflected entity as an annotation of type `Tag` (meaning the factory has registered the type)
    - The reflected entity is a derived class of `Base`
    - The reflected entity matches the name of the class in source code (with or without the `::` symbols) or matches the Mapping annotation.

It then forwards the rest of the arguments to the class's constructor 

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
