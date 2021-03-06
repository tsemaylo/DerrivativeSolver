# Design notes

## Overview
Basically the application consists of the following elements:

* **SolverApplication** is the application object by itself. It is responsible 
for interaction with user and managing activities relevant for the user.
* **MathParser** is responsible for parsing of the string input from user. 
It accepts string expression and builds syntax tree - the hierarchical data structure 
representing the parsed expression. This component provides the interface to parse 
the string, syntax tree and proposes the interface for custom operations to be performed with syntax tree.  
* **Visitors** are implementations of operations over syntax tree: like differentiation or output in user-readable form.

## Design goals and priorities
- Taste the c++ and get the experience with actual standards
- Keep design as clean as possible

## Technical constraints and considerations

* using of c++14
* gcc + plain Makefiles
* smart pointers instead of explicit memory management.
* keep syntax tree element as simple as possible, PODs would be great
* -Wall -Werror -Wpedantic
* minimize inheretance, nevertheless it can be essential to use it in some way.
* exceptions instead of error codes

## Design of the application

The design, as it usually happens, is evolving during the development. 
The initial model of the application are preserved here: [Initial design](initialDesign.md)
Here you can find the actual structure of the application. 

The basic elements of design, as it was mentioned above, are: application object, 
parser of mathematical expressions and visitors as operations for expressions.

The sequence of actions to be performed to differentiate an expression given by user is following:

No. | Activity                             | Component   | Input                  |  Output 
----|--------------------------------------|-------------|------------------------|---------------
1   | Get input from user                  | Application | Command line arguments | String
2   | Parse an expression from user        | MathParser  | String from #1         | Syntax tree   
3   | Differentiate the expression         | Visitors    | Syntax tree from #2    | Syntax tree 
4   | Optimize ans simplify the expression | Visitors    | Syntax tree from #3    | Syntax tree 
5   | Get string representation of result  | Visitors    | Syntax tree from #4    | String 
6   | Print result                         | Application | String from #5         | Command line output 

The relationships between items and basic decomposition is provided on the picture below.

![General decomposition](img/designItemsRelationships.png)

It is, for sure, a very coarse representation of the application structure. 

**SolverApplication** on this picture represent the application component.

The **MathParser** component implements the parsing mechanism 
and exposes the interface to the application. It also provides the data structures 
representing the syntax tree (**Expression**) and predefines the interface (**Visitor**) to implement 
operations over this syntax tree like differentiation or optimitzation/simplification of the structure.

The component **Visitors (operations)** contains implementation of Visitor interface which is used by **SolverApplication**.

### Design of the MathParser component

The parser implements parsing logic according to the grammar. The grammar is built of 
**Rule**s which are implementing certain [grammar rules](grammar.md). The parser implements the 
"Shift-Reduce" method ~~, at least some sort of it,~~ because of the simplicity of implementation. 
With this approach the high performance can't be anticipated and it is not a priority so far.  

The parser can be viewed as independent entity and distributed in the form of library,
which can be eventually used by more than one application.

Public interface of the **MathParser** is built of:

* Interface **Parser**
* **Expression** and all its children
* Service functions: 
  * Factories for parser and to build the syntax tree.
  * Generating the string representation of the syntax tree.
* **Visitor** interface

Detailed decomposition of this component into classes is provided on the picture below.

![Parser of mathematical expressions](img/designParser.png)

The diagram below explains the structure of rules based on the [grammar description](grammar.md).

![Parser-Rule](img/parserRule.png)

Rules are divided into two categories: ones that handle arythmetic operations with arguments on
both sides of operation sign and functions with one argument. Any other rule besides of these 
categories can be established by means of deriving a new class from **Rule** in order to preserve 
the interface, which allows extensions of Parser according to OCP. The order of rule execution 
is determined by the position in the **Parser::grammar** list. **IMPORTANT:** position in the grammar list
does not define the priority of operations!

"Syntax tree" is a data structure which represents the parsed expression. 
It has **Expression** as a base class and the following element are representing
syntax tree items.

![Class diagram](img/syntaxTree.png)


### Design of the Visitors

The relationship between the **Visitor** and **Expression** is basically the classical 
Visitor design pattern from GoF, that maintainability advantages and adheres to OCP - adding 
new syntax tree element leads to adding an new method to the Visitor interface.
Adding a new operation considers adding a new **Visitor** implementation for all related 
expression elements.

The following operations are distinguished:
* Differentiation of the expression (**Differentiator**),
* Simplification of the expression to avoid nonsense expressions like __X*1__ instead of just  __X__ (**Optimizer**),

![Class diagram](img/visitors.png)


Optimizer uses a set of rules, which implement simplification heuristics for particular cases.
The following rules are distinguished:

 Name                        | Applied to | Use case                                            
-----------------------------|------|----------------------------------------------------------
SumWithNullArgumentRule      | Sum  | e + 0 => e, where e is a random **Expression**.
SumWithNegativeRule          | Sum  | a+(-1*b) => a-b, where a and b are random **Expressions**.
SumIdenticalExpressionsRule  | Sum  | N*e + M*e => (N + M)*e, where N and M are **Constant**s and e is a random **Expression**.
SumConstantsRule             | Sum  | a+b => calculated result, where a and b are **Constant**a.
PowOfPowRule                 | Pow  | (x^m)^n => x^mn <br> (1/x^n)^m => (x^-n)^m = x^-mn <br>, where x,m and n are random **Expression**s.
PowConstantRule              | Pow  | A^B -> calculate results <br> e ^ 0 = > <br>  e ^ 1 => e <br>, where a and b are **Constant**s and e is a random **Expression**.
MultWithNumeratorRule        | Mult | a*(b/c) => (ab)/c, where a, b and c are random expressions.
MultQuotientsRule            | Mult | a*(b*x) => ab*x <br> a*(b/x) => ab/x <br>, where a and b are **Constant**s and x is a random **Expression**. <br> Also considering different combinations of arguments.
MultNumeratorDenominatorRule | Mult | ax^n*b/x^m => (a*b)*x^(n-m), if n > m <br> ax^n*b/x^m => (a*b)/x^(m-n), if m > n <br>, where a, b, x are random **Expression**s and N and M are **Constant**s.
MultIdenticalExpressionsRule | Mult | A*e^M * B*e^N => AB*e^(M+N), where A, B are **Constant**s and e, M, N are random **Expression**s.
MultConstantsRule            | Mult | a*b => calculate results, where a nd b are **Constant**s.
LnOfExpRule                  | Ln   | ln(exp(x)) => exp(x), where x is a randon Expression.
FunctionEvaluateRule         | Sin, Cos, Tan, Ctan, Ln, Exp | Evaluation of the function of a **Constant**.

Note that optimization rules for **Div** and **Sub** are not defined because they can be derived from already existing **Mult** and **Sum** fules.

### Design of the Application

**SolverApplication** object which handles interaction with user, provides entry point and performs error handling.

Since no complex execution logic or sophisticated recovery after errors are considered, 
it is decided to to error handling on the level of **Application**.

The only one reasonable error handling here is just printing an error information to the user,
and this is the responsibility of **Application**.

## Testing
Two types of testing are considered:

**Unit testing**

There are some test cases maintained during development. 
Say ``` make clean all ``` and available tests will be run automatically during the build process.
The project incorporates [Google's C++ Test Framework](https://github.com/google/googletest "Google Test").
The naming of test cases is based on these ["Naming standards for unit tests"](http://osherove.com/blog/2005/4/3/naming-standards-for-unit-tests.html).

**Acceptance tests**

Represented as an automated script which performs testing of application against most relevant input data.
Performed in the build time.


## Post-mortem

Short analysis and review summary of final design and implementation can be found in [final report](postMortem.md).