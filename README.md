# Dynamic State Tree

Dynamic State Tree is a simple plugin that contians implementation of a K2_Node that allows user to create a state tree reference structure and set all its exposed parameters in runtime.

By default you can create state tree reference as a property and set it's parameters. But you can only set them to a constant value. See below screenshot.

![Default State Tree Reference](/Screenshots/default-state-tree-ref.png?raw=true "Optional Title")

The Dynamic State Tree node solves this problem by creating an instanced property bag and allowing user to set a property to either a constant value or hook it up to a pin which will be resolved at runtime. See screenshot below for an example.

![Dynamic State Tree Reference](/Screenshots/dynamic-state-tree-ref.png?raw=true "Optional Title")

## Known limitations
1. Due to the nature of the blueprint graph nodes, there is no way to allow user to mark which parameters they want to override and which they don't. Because of that, **ALL** parameters will **ALWAYS** be overwritten with the values defined on the node.
2. Currently structs are not being "unpacked" and need to be passed by reference which might be inconvenient in some cases (e.g. there is no way of defining transform as set of vector, rotation and scale; instead user must use `Make Transform` blueprint node and plug it into the node's pin).
