# datadump scripting language

* [Introduction](#introduction)
* [Scripting basics](#scripting-basics)
    * [Variables](#variables)
    * [Assignment lines](#assignment-lines)
    * [Loops](#loops)
    * [Printing lines](#printing-lines)
* [Transforms](#transforms)
    * [Arithmetic/logic transforms](#arithmeticlogic-transforms)
    * [Checking transforms](#checking-transforms)
    * [Data transforms](#data-transforms)
    * [Type conversion transforms](#type-conversion-transforms)
* [Sample scripts](#sample-scripts)
    * [Example 1: pointer parsing script](#example-1-pointer-parsing-script)
    * [Example 2: text dumper](#example-2-text-dumper)

***


## Introduction

The purpose of the scripting language is to process a single binary inclusion (incbin) in a programmable way, thus enabling complex forms of processing and dumping its contents.

A script is executed taking the incbin's data as input. The script can consume as much data from it as it needs to. As long as data remains, the script will be continuously re-run on the remaining data (although no state is preserved between successive executions).

Scripts are plain text files containing statements to be executed. Each line contains one statement; there is no statement separator. There are six kinds of lines recognized by the parser (their syntax will be explained in detail in their own sections):

- **Blank**: a line that contains nothing but whitespace (spaces and/or tabs). Such a line is completely ignored.
- **Assignment**: a line that computes a value and assigns it to a variable. Its first non-whitespace character must be `$`.
- **Loop start**: a line that begins a loop, which is the only control structure the scripting language has. (Loops can be used to simulate if-then constructs as well.) Its first non-whitespace characters must be `:$`.
- **Loop end**: a line that ends a loop, going back to its corresponding loop start; it behaves like a closing brace. Its first non-whitespace characters must be `::`.
- **Comment**: a line whose first non-whitespace character is `:`, but neither `$` nor `:` follows immediately without intervening whitespace. It is treated as a comment and ignored completely.
- **Printing line**: a line whose first non-whitespace character is neither `:` nor `$`. It will be printed to the output after variable substitution; this is the way in which a script outputs data.

Note that printing lines are buffered and will only be actually printed after the script runs successfully to completion and no data remains for it to consume (i.e., after it finishes iterating). If any error occurs in any of the script iterations, the output data is discarded and the error is printed to the user instead, thus allowing the user to take a different action for that incbin. This behavior can also be used to avoid dumping the incbin's contents — deliberately introducing an error (for instance, the line `$ 0 requirenot:0` will always fail) will prevent execution from continuing and output from being dumped.

The following sections of this page explain the exact syntax and behavior for each kind of line; full sample scripts will be located at the end of the document.


## Scripting basics

### Variables

Scripts can define and use an arbitrary number of variables. All variable names begin with `$`, followed by an identifier; valid characters for an identifier are letters (A-Z, a-z), digits (0-9) and the underscore. Thus, names like `$var` or `$This_is_variable_number_2` are valid. (Variable names may also begin with digits, such as `$20` or `$4th_var`; the first character needn't be a letter.)

Variables may contain values of four different data types:

- **Integers**: 8-bit, 16-bit or 32-bit values. They are always treated as signed unless otherwise specified. (If needed, 8-bit and 16-bit integers can be cast to 32-bit as unsigned, by applying two transforms: `$32bit $8bit int:4 and:0xff` or `$32bit $16bit int:4 and:0xffff`.)
- **Arrays**: arrays of integers. Arrays can also be 8-bit, 16-bit or 32-bit; they are however restricted to only containing one type of integers.
- **Data**: raw data. The script's input (the incbin contents) are of this type. It is a stream of bytes of known length.
- **Text**: text data. It is similar to the previous one, but it will be interpreted as text when printed, and only a handful of operations are available for it.

The pseudo-variable `$` is initialized when the script starts with the current incbin data (which will be all of it for the first execution, or the remaining part of it for successive ones). Assigning to `$` is ignored, which makes it useful for evaluating expresions where the result should be discarded (for instance, `$ $length checkmax:20` will validate that `$length` is less than or equal to 20 (causing an error if it isn't), but it won't store the result anywhere).


### Assignment lines

An assignment line assigns a value to a variable. As assignment line consists of a variable, an initial value and zero or more transforms, all separated by any amount of whitespace (spaces and/or tabs). For instance, this is a valid assignment line: `$five 2 multiply:2 add:1`. In that case, `$five` is the variable assigned to, `2` is the initial value, and `multiply:2 add:1` are the two transforms.

The initial value can be an integer (decimal, hexadecimal (leading `0x`) or octal (leading `0`)), which will be interpreted as 32-bit; another variable, or a read expression. A read expression is of the form `:<value>` or `::<value>`, where `<value>` is a variable, an integer, or empty (for instance, `:20` or `::$length`), and it consumes data from the input; this is the only way to do so. A read expression evaluates to a value of type data containing as many bytes as its parameter (which implies that, if a variable is used, that variable must be of type integer), and it also advances the input stream (and the `$` variable) by that many bytes; if the parameter is empty (i.e., the read expression is just `:` or `::`), the read expression reads all the remaining data. Reading a negative amount of bytes or an amount greater than the remaining bytes in the input stream will cause an error. If the read expression begins with `::` (as opposed to just `:`), it will emit an `.incbin` directive corresponding to the bytes read as a side effect; this can be used to leave some data unprocessed. (The data will still be read and made available to the script.)

Any amount of transforms can be specified; those transforms will be applied in sequence to the initial value before assigning it to the variable. A transform is always in the form `name:value`, where `name` is the name of the transform to apply, and `value` is either an integer or a variable of type integer. Available transforms will be described later in this document. Note that no whitespace is allowed within a transform: `add: 1` is a syntax error.


### Loops

Loops are begun with a line of the form `:$variable` and ended with a line of the form `::`. Any line beginning with `:$` starts a loop, and will iterate over its contents as long as the contents of `$variable` are true; if they are false, execution will continue on the line that follows the corresponding loop closing. A line beginning with `::` (the rest of its contents don't matter) will end the most recent loop not yet closed, making loop nesting natural. Nested loops may look like this:

    :$outer_loop_test
      : some code
      :$inner_loop_test
        : inner loop code
      ::
      : more code
    ::

Depending on its type, a variable is considered true in the following cases:

- **Integer**: if its value is non-zero
- **Data**, **text**: if it contains data (i.e., its length is non-zero)
- **Array**: if it contains any elements

Note that a loop may be used to create an if-then construct by assigning 0 to the loop test variable within its body. For instance:

    $condition $value compare:46 forcemin:0
    : the previous line will set $condition to 0 if $value <= 46, or 1 if $value > 46
    :$condition
      The value is greater than 46.
      $condition 0
    ::


### Printing lines

Printing lines create output, and thus are typically the main lines of scripts. Any line that doesn't begin with a `$` or `:` (not counting leading whitespace) will be considered a printing line. Within those lines, variable substitutions are replaced by their contents; everything else is printed literally as-is.

A variable substitution is simply a variable name, with an optional `+`, `-`, `%` or `*` sign between the `$` sign and the rest of the name. Therefore, `$var`, `$+var2` and `$-var3` are valid variable substitutions. Every character that comes after `$`, `$+`, `$-`, `$%` or `$*` and is a valid variable name character (A-Z, a-z, 0-9, underscore) is considered part of the variable substitution; the substitution ends as soon as a different character (or the end of the line) is encountered. If the substitution ends in a `$` character, this character is ignored when printing (e.g., `$var$` will print the same as `$var`); this allows using variable substitutions that are immediately followed by valid variable name characters.

For instance, assuming that the variables `$one` through `$ten` contain 32-bit integer values according to their names, the line `$+two + $+three is $+five, which is not $eight` will print `2 + 3 is 5, which is not 0x00000008`. Note that it is an error to use an undefined variable here.

The way the values are printed depends on their type and on whether a `+`, `-` or `*` sign was used, as follows:
- **Integer**: it is printed in hexadecimal, with a leading `0x` and using two, four or eight characters according to the value's size (8-bit, 16-bit or 32-bit). If a `+` or `-` sign was used in the substitution, the number is printed in decimal instead, treated as unsigned or signed respectively. If a `*` sign was used in the substitution and the value is 32-bit, it is processed as a pointer and printed in the same way the `dataptr` command (from interactive mode) would; narrower integers ignore the symbol. If a `%` sign was used in the substitution, the UTF-8 character whose codepoint corresponds to the unsigned value of the variable is printed; zeros, surrogates and out-of-range characters cause nothing to be printed.
- **Array**: each value is printed as an integer, as specified above. If the array contains two or more values, the values will be separated by a comma-space pair. If a `%` sign was used in the substitution, each element of the array is converted to its corresponding UTF-8 codepoint (ignoring invalid values as stated above) and the resulting string is printed; surrogate pairs are handled as a single codepoint instead of being discarded.
- **Data**: it is printed as an hexadecimal string, such as `01ff02fe03fd04fc`. If a `%` sign was used in the substitution, the data is printed raw, with null bytes removed.
- **Text**: it is parsed the usual way and printed, the same way as string data usually is. Note that this does not include quotes, so those should be added manually (e.g., `"$text"`).


## Transforms

The scripting language defines a series of transforms, which act as functions that turn a value into another based on a parameter. For instance, the `add` transform will add its parameter to its value, so that `add:2` applied to a value of 3 will result in 5.

Available transforms will be described in the following sections, grouped by type.


### Arithmetic/logic transforms

These transforms can only be applied to integers or arrays (applying them to other types results in an error), and they always return a value of the same type as the original value. Applying them to an array applies the transform to each element of the array individually.

**add**: adds the parameter to the value  
**subtract**: subtracts the parameter from the value  
**rsub**: subtracts the value from the parameter  
**multiply**: multiplies the value by the parameter  
**divide**: divides the value by the parameter (division by 0 is undefined)  
**rdiv**: divides the parameter by the value  
**modulo**: calculates the remainder of the division between the value and the parameter  
**rmod**: calculates the remainder of the division between the parameter and the value  
**and**: applies a bitwise AND between the value and the parameter  
**or**: applies a bitwise OR between the value and the parameter  
**xor**: applies a bitwise XOR between the value and the parameter  
**shl**: shifts the value left by the amount specified by the parameter  
**asr**: shifts the value right by the amount specified by the parameter, sign-extended  
**shr**: shifts the value right by the amount specified by the parameter, zero-extended (note: this will produce unexpected results on values smaller than 32 bits)  
**forcemin**: returns the value if greater than or equal to the parameter, otherwise returns the parameter  
**forcemax**: returns the value if less than or equal to the parameter, otherwise returns the parameter  
**forceminu**, **forcemaxu**: 32-bit unsigned versions of the above (note: these will produce unexpected results on values smaller than 32 bits)  
**compare**: returns -1, 0 or 1 if the value is respectively smaller, equal or larger than the parameter  
**compareu**: 32-bit unsigned version of the above (note: this will produce unexpected results on values smaller than 32 bits)

Note that the `compare` and `compareu` transforms can be used to generate a value suitable for a loop test. For instance:

- Equal: `compare:46 and:1 xor:1`
- Not equal: `compare:46`
- Less than: `compare:46 forcemax:0`
- Less than or equal: `compare:46 subtract:1`
- Greater than: `compare:46 forcemin:0`
- Greater than or equal: `compare:46 add:1`

Using 32-bit unsigned transforms on smaller values may result in unexpected results, because values will be converted to 32-bit before being treated as unsigned. Therefore, an 8-bit value of `0xfe` (-2) will be converted to 32-bit, getting `0xfffffffe` (-2 again), and then treated as unsigned obtaining `4294967294` (instead of the probably expected `254`).


### Checking transforms

These transforms can be applied to integers or arrays (applying them to other types results in an error). Their purpose is to verify that the value fulfills a certain criterion; they return the original value unmodified if it does, and they raise an error if it doesn't. Applying them to arrays results in the transform being applied to each member of the array individually. The same caveat for 32-bit unsigned transforms explained in the previous section applies here.

**checkmin**: checks that the value is greater than or equal to the parameter.  
**checkmax**: checks that the value is less than or equal to the parameter.  
**checkminu**, **checkmaxu**: 32-bit unsigned versions of the above.  
**checkptr**: checks that the value is a valid pointer, should only be applied to 32-bit integers and arrays. Valid pointers are in the following areas: 0x02000000–0x0203ffff, 0x03000000–0x03007fff, 0x08000000–0x09ffffff. If the parameter is zero, null pointers are also allowed; if it is non-zero, they are forbidden.  
**require**: checks that the value is equal to the parameter.  
**requirenot**: checks that the value is not equal to the parameter.


### Data transforms

These transforms manipulate data; therefore, they cannot be applied to integer values, and doing so causes an error. All data counts are specified in number of items: that's bytes for data and text values, or elements for arrays. Specifying a negative data count or a count larger than the size of the value results in an error.

**copy**: creates a new value copying the first N items of the value, where N is the parameter.  
**skip**: creates a new value copying all but the first N items of the value, where N is the parameter.  
**item**: returns the item in the Nth position (first item: 0), where N is the parameter. Can only be applied to arrays, gives an error for other types.  
**append**: can only be applied to array values (applying it to other types results in an error). Returns a new array with the integer corresponding to the parameter added at the end of it.  
**prepend**: can only be applied to array values (applying it to other types results in an error). Returns a new array with the integer corresponding to the parameter added at the beginning of it.  
**rotate**: can only be applied to array or data values (applying it to other types results in an error). Returns a new value with the first N items (where N is the parameter) removed and reinserted at the end, rotating the value. ("Items" refer to elements for array-type values, or bytes for data-type values.) Negative parameters make this transform behave like `rotateback` with a positive parameter; excessively large parameters are reduced modulo the length of the value.  
**rotateback**: is identical to `rotate`, but rotates in the opposite direction.  
**byteswhile**: can only be applied to data values (applying it to other types results in an error), returns the length of the longest initial segment where all bytes are equal to the parameter (for instance, for a data of `01 01 01 02 03 02 01 00` and a parameter of 1, it will return 3, since it begins with three `0x01` bytes). If the parameter doesn't match the first byte of the data, it returns 0. The parameter must be between -128 and 255.  
**bytesuntil**: similar to the previous one, but it returns the length of the longest initial segment where all bytes are NOT equal to the parameter. If the first byte is equal to the parameter, it will return 0; if the parameter byte is not found at all within the data, it will return the data's length in bytes.  
**length**: returns the length of the value, in units specified by the parameter. If the parameter is 1, 2 or 4, the length is returned in bytes, halfwords or words respectively; an error is raised if the length is not divisible by the parameter. If the parameter is 0, the length is returned in number of items for non-text values, or in converted length (that is, length after parsing) for text values. Any value for the parameter other than 0, 1, 2 or 4 raises an error.  
**count**: returns the amount of items (bytes for data and text values, elements for array values) in the value that are equal to the parameter, as a 32-bit integer.  
**any**: returns 1 if any of the items (bytes for data and text values, elements for array values) in the value are equal to the parameter, or 0 otherwise. (This transform is added for completeness, as `any:$value` achieves the same effect as `count:$value forcemaxu:1`.) For an empty value, it returns 0, since none of the items are equal to the parameter.  
**all**: returns 1 if all of the items (bytes for data and text values, elements for array values) in the value are equal to the parameter, or 0 otherwise. For an empty value, it returns 1, since all of the items are equal to the parameter (in other words, no item in the value is not equal to it).


### Type conversion transforms

These transforms convert data from one type to another. They are necessary to generate values of types other than data, since the input is a single data-type value.

**int**: converts the value to an integer, of the width specified by the parameter; acceptable parameters are 1, 2 and 4 for 8-bit, 16-bit and 32-bit values respectively (other values for the parameter raise an error). If the value is an integer, this simply casts the value to an integer of a different type. If the value is an array, the operation is applied to the whole array, generating an array of a different type. If the value is of type data, then as many bytes as the parameter specifies are taken from the beginning of it and converted to an integer in little-endian form; an error will be raised if there aren't enough bytes. Applying this transform to a value of type text will raise an error.  
**multi8**, **multi16**, **multi32**: create arrays. Applying these transforms to a value that isn't of type data or integer will raise an error. An array, of a type determined by the transform and an item count determined by the parameter, will be created: if the value is of type data, by reading enough bytes from the beginning of the value (an error will be raised if there isn't enough data); and if the value is of type integer, by repeating that integer across all items of the new array. An error will be raised if the parameter is negative.  
**multi**: create arrays from data or vice-versa. Valid values for the parameter are 0, 1, 2 or 4; other values cause an error. If the parameter is 1, 2 or 4, it will create a array from data (as above); in this case, the integer width is the parameter, and the length will be the length of the data value (which must be a multiple of the parameter). If the parameter is 0, it will undo this conversion, creating a data value from an array. Applying this transform to a value of an incorect type (i.e., a type other than data with a non-zero parameter, or a type other than an array with a zero parameter) will raise an error.  
**text**: create text from data. Applying this transform to a value that isn't of type data will raise an error. A value of type text will be created from the data at the beginning of the value, with its length in bytes specified by the parameter; it is an error to specify a negative parameter, or a parameter that is greater than the value's length in bytes. An error will also occur if the data cannot be parsed as text.


## Sample scripts

The following scripts show the basic features of the scripting language by example. This does not act as a substitute for the reference above.

#### Example 1: pointer parsing script

The following script will turn a pure pointer incbin into the pointers that constitute it:

    $ptr :4 int:4 checkptr:1
    .4byte $ptr

The first line reads 4 bytes from the incbin stream (`:4`), converts those 4 bytes into a 32-bit integer and validates that the resulting number is indeed a valid pointer. The second line prints it. Since the script is executed repeatedly until all data is consumed, this will output all pointers from the incbin; if the length isn't a multiple of 4, or some of the values it contains aren't valid pointers, an error will be raised and the incbin will not be replaced.

If, for any reason, it is desired to also output the amount of pointers read, then a loop is needed to output all the pointers in one execution of the script, since they must be counted. This script does the job:

    $count 0
    :$
      $ptr :4 int:4 checkptr:1
      .4byte $ptr
      $count $count add:1
    ::
    @$+count pointers written

The first line initializes `$count` to 0. The following line begins a loop while `$` is true, that is to say, while it still contains data. Within the loop, the first two lines are the same as in the previous case (read a pointer and output it), and the following one increments the `$count` variable by 1. Finally, after the loop ends (`::`), the amount of pointers is output.

An alternative to this is to read the pointers into an array, like so:

    $pointers : multi:4 checkptr:1
    $count $pointers length:0
    .4byte $pointers
    @$+count pointers written

The first line reads the whole input into an array of 32-bit integers (which will fail if the input isn't a multiple of 4 bytes) and ensures that all of those values are valid pointers. The second line obtains the amount of pointers in the array. The third line prints the pointers, and the fourth line prints the count. Unlike the previous scripts, this script will print all of the pointers in the same line, separated by commas (instead of one per line).

#### Example 2: text dumper

The following script will dump all text strings within an incbin.

    $length $ bytesuntil:0xff add:1 checkmax:1000
    $text :$length text:$length
    .string "$text"

The first line counts how many bytes there are until the first `0xff` byte, adds 1 (to include the terminator itself) and ensures that the length is no bigger than 1,000 bytes. The following line reads as many bytes from the input as the length calculated above and converts that to text. Finally, the text is printed.

Again, since this script will be repeatedly executed until no data is left, this will print all strings.

If the `.string` macro accepts an argument to indicate the padding size (assuming that this padding is done with `0xff` bytes), this can be calculated as well:

    $text_length $ bytesuntil:0xff checkmax:999
    $padded_length $ skip:$text_length byteswhile:0xff add:$text_length
    $text :$padded_length text:$text_length
    .string "$text", $+padded_length

The first line calculates the text length as usual (this time not including the terminator). The following line will calculate the padded length by taking the input data, skipping over the text (since the length is known by now), and then calculating how many `0xff` bytes in a row there are after the skipped text; finally, that is added to the text length to obtain the full padded length. The third line reads the text with the padding from the input (to remove both from the input stream), but only converts the read data to text up to the text length, thus effectively discarding the padding. Finally, the fourth line outputs the text and the total length (including the padding) for the macro to use.
