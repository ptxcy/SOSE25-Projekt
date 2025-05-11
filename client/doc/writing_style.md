# Writing Style

## Variable Naming Style

```c++
public:
    u8 public_variable_snake_case;
private:
    u8 m_PrivateVariableCamelCase;

inline u8 g_GlobalStaticVariable;
u8 _local_static_variable;

u32 public_function()
{
    u8 __TemporaryVariableUnderscore;
    void* p_PointerToMemory;
    u8& p_ReferenceToVariable;
    return return_variable_snake_case;
}

u32 _private_function() {  }
```


## Brackets

```c++
// loops & conditions
for/while/if () // ...oneline uses no brackets
{
    // ...multiline use newline brackets
}

// switch & case
switch (cond)
{
case 1:  // case has same indentation depth as switch
    // things happen
    break;
};

// definitions
structure = {
    // ...definitions use sameline brackets
};
```


## Documentation

```c++
/**
 *	function description
 *	\param a: explanation first parameter
 *	\param b: explanation second parameter
 *	\param c: explanation last parameter
 *	\returns value description that will be return upon running
 */
u8 func(u8 a,u16 b,string c) {  }
```


## Details

```c++
Pointer* p_StarAtType;
Reference& p_ReferenceAtType;
template<typename T> void template_sameline_as_function(T a);
function_with_a_lot_of_parameters(parameter1,parameter2,parameter3
                                  parameter4,param5,param6,param7,param8
                                  parameter9,parameter10,anothermeter11);
```