import glob

res = """#pragma GCC optimize("O3")\n"""
res += "#include <cstdlib>\n"
res += "#include <cstddef>\n"
res += "#include <string>\n"
res += "#include <vector>\n"

def comp(x):
    if x.endswith("util.h"): return 0
    if x.endswith("nameid.h"): return 1
    if x.endswith("lexer.h"): return 2

    return 999999

headers = [e for e in glob.glob("cotton_lib/**", recursive=True) if e.endswith(".h")]
headers.sort(key=comp)
sources = [e for e in glob.glob("cotton_lib/**", recursive=True) if e.endswith(".cpp")]


for file in headers:
    with open(file, "r") as fd:
        res += "// HEADER " + file + "\n"
        res += fd.read()
        res += "\n"

for file in sources:
    with open(file, "r") as fd:
        res += "// SOURCE " + file + "\n"
        res += fd.read()
        res += "\n"


res = res.splitlines(keepends=True)
new_res = []

# removes lines that include project files
for line in res:
    if line.startswith("#include \""):
        continue
    new_res.append(line)

res = new_res.copy()

# removes comments
import re
res = "".join(res)
res = re.sub('//.*?\n|/\*.*?\*/', '', res, flags=re.S)


# removes all spaces
for i in range(0, 50): 
    res = res.replace("  ", " ")


res += '''
/*
 Copyright (c) 2024 Ihor Lukianov (lis05)

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <chrono>
using namespace std::chrono;
using namespace Cotton;
using namespace Cotton::Builtin;

void emergency_error_exit() {
    exit(1);
}

// TODO: add --profiler flag

int main(int argc, char *argv[]) {

const char *src = R"(
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

arr = make(Array)
    .resize(read(Integer))
    .apply(function(x){x=read(Integer);});

target = arr.copy().sort(function(a,b){a>b;})[1];

// don't have a function that would return element's position yet
for i = 0; i < arr.size(); i++; {
    if arr[i] == target; {
        println(i + 1);
        return;
    }
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";

    ErrorManager em(emergency_error_exit);
    Lexer        lx(&em);
    Parser       pr(&em);
    NamesManager nmgr;

    auto strsrc = std::string(src);

    auto tokens = lx.process(strsrc);
    for (auto &token : tokens) {
        token.nameid = nmgr.getId(token.data);
    }
    auto program = pr.parse(tokens);

    GCDefaultStrategy gcst;
    Runtime           rt(&gcst, &em, &nmgr);
    auto res        = rt.execute(program, false);
    delete program;
}

'''

# reserves special keywoards so that they dont break
res = res.replace("#if", "__I_F_D_I_R_E_C_T_I_V_E__")
res = res.replace("#ifdef", "__I_F_D_E_F_D_I_R_E_C_T_I_V_E__")
res = res.replace("#ifndef", "__I_F_N_D_E_F_D_I_R_E_C_T_I_V_E__")
res = res.replace("#endif", "__E_N_D_I_F_D_I_R_E_C_T_I_V_E__")

# replaces all these words with shorter ones
keywords =  '''
verifyExactArgsAmountFunc
verifyExactArgsAmountMethod
verifyMinArgsAmountFunc
verifyMinArgsAmountMethod
verifyIsInstanceObject
verifyIsValidObject
verifyIsOfType
verifyIsTypeObject
verifyHasMethod
isValidObject
isTypeObject
isInstanceObject
isOfType
Runtime
ErrorManager
HashTable
Cotton
NamesManager
NameId
Token
token
Object
Instance
Type
Lexer
Parser
getIntegerValueFast
getBooleanValueFast
getStringValueFast
getArrayValueFast
signalError
hasMethod
getContext
newContext
popContext
ProfilerCAPTURE
protectedBoolean
protectedNothing
Adapter
execution_result_matters
instance
builtin_types
value
OPEN_BRACKET
CLOSE_BRACKET
TYPE_KW
OPEN_CURLY_BRACKET
CLOSE_CURLY_BRACKET
SEMICOLON
METHOD_KW
FUNCTION_KW
COMMA
PLUS_PLUS_OP
MINUS_MINUS_OP
OPEN_SQUARE_BRACKET
CLOSE_SQUARE_BRACKET
DOT_OP
AT_OP
PLUS_OP
MINUS_OP
NOT_OP
INVERSE_OP
MULT_OP
DIV_OP
REMAINDER_OP
RIGHT_SHIFT_OP
LEFT_SHIFT_OP
LESS_OP
LESS_EQUAL_OP
GREATER_OP
GREATER_EQUAL_OP
EQUAL_OP
NOT_EQUAL_OP
BITAND_OP
BITXOR_OP
BITOR_OP
AND_OP
OR_OP
ASSIGN_OP
ASSIGN_PLUS_OP
ASSIGN_MINUS_OP
ASSIGN_MULT_OP
ASSIGN_DIV_OP
ASSIGN_REMAINDER_OP
BOOLEAN_LIT
NOTHING_LIT
WHILE_KW
FOR_KW
IF_KW
ELSE_KW
CONTINUE_KW
BREAK_KW
RETURN_KW
UNSCOPED_KW
STRING_LIT
CHAR_LIT
INT_LIT
REAL_LIT
IDENTIFIER
ExprNode
FuncDefNode
TypeDefNode
OperatorNode
AtomNode
ParExprNode
IdentListNode
StmtNode
WhileStmtNode
ForStmtNode
IfStmtNode
ReturnStmtNode
BlockStmtNode        
TextArea     
fields
field
methods
method
POST_PLUS_PLUS
POST_MINUS_MINUS
CALL
INDEX
DOT
AT
PRE_PLUS_PLUS
PRE_MINUS_MINUS
PRE_PLUS
PRE_MINUS
NOT
INVERSE
MULT
DIV
REM
RIGHT_SHIFT
LEFT_SHIFT
PLUS
MINUS
LESS
LESS_EQUAL
GREATER
GREATER_EQUAL
EQUAL
NOT_EQUAL
BITAND
BITXOR
BITOR
AND
OR
ASSIGN
PLUS_ASSIGN
MINUS_ASSIGN
MULT_ASSIGN
DIV_ASSIGN
REM_ASSIGN
TOTAL_OPERATORS
OperatorId
saveState
restoreState
highlight
highlightNext
canPassThrough
hasNext
checkNext
hasPrev
rollback
ParsingREsult
success
error_message
EXPR
FUNC_DEC
TYPE_DEF
OPERATOR
EXPR
PAR_EXPR
IDENT_LIST
METHOD_DEF
STMT
WHILE_STMT
FOR_STMT
IF_STMT
CONTINUE_STMT
BREAK_STMT
RETURN_STMT
BLOCK_STMT
acknowledgeTrack
acknowledgeUntrack
acknowledgeEndOfCycle
acknowledgePing
object
instance
master
scope
arguments
Builtin
data
begin_pos
end_pos
caller
selector
dot
atom
ident
nameid
filename
runOperator
runMethod
clearExecFlags
setExecFlagNONE
setExecFlagDIRECT_PASS
setExecFlagCONTINUE
setExecFlagBREAK
setExecFlagRETURN
isExecFlagNONE
isExecFlagDIRECT_PASS
isExecFlagCONTINUE
isExecFlagBREAK
isExecFlagRETURN
spreadMultiUse
popScopeFrame
newScopeFrame
getStringDataFast
makeBooleanInstanceObject
makeStringInstanceObject
makeIntegerInstanceObject
makeReadInstanceObject
makeArrayInstanceObject
makeNothingInstanceObject
makeFunctionInstanceObject
makeCharacterInstanceObject
BooleanInstance
StringInstance
IntegerInstance
NothingInstance
RealInstance
ArrayInstance
FunctionInstance
CharacterInstance
Character
BooleanType
StringType
IntegerType
NothingType
RealType
ArrayType
FunctionType
CharacterType
getRealValueFast
getIntegerValueFast
getScope
addVariable
getId
getString
nmgr
addMethod
MagicMethods
OperatorInfo
LEFT_TO_RIGHT
RIGHT_TO_LEFT
ParsingResult
consume
state
operator_
cur_priority
cur_associativity
EMPTY_PRIORITY
expr_stack
expr
while_stmt
for_stmt
if_stmt
block_stmt
parseExpr
parseStmt
parse
verify
error_manager
printColor
last_line_position
messages
string_begin
string_end
intersection
GCDefaultStrategy
GCStrategy
num_tracked
sizeof_tracked
ops_cnt
checkConditions
tracked_instances
tracked_objects
tracked_types
track
untrack
execution_flags
ExecutionFlags
NONE
CONTINUE
BREAK
RETURN
DIRECT_PASS
mm__make__
mm__copy__
mm__bool__
mm__char__
mm__int__
mm__E11__
mm__string__
mm__repr__
mm__read__
UnaryOperatorAdapter postinc_op postdec_op preinc_op predec_op positive_op negative_op not_op
 inverse_op
 BinaryOperatorAdapter mult_op div_op rem_op rshift_op lshift_op add_op sub_op lt_op leq_op gt_op
 geq_op eq_op neq_op bitand_op bitxor_op bitor_op and_op or_op
 NaryOperatorAdapter call_op index_op
 getMethod
addOperator
'''
head = ""
letter = 0
num = 0

list = keywords.split()
list.sort(key=lambda x: len(x), reverse=True)

for word in list:
    if not word:
        continue
    
    id = chr(letter + ord('A')) + str(num)
    if len(word) > len(id):
        res = res.replace(word, id)

    letter = letter + 1
    if letter == 26:
        letter = 0
        num = num + 1

# restores reserved keywords

res = res.replace("__I_F_D_I_R_E_C_T_I_V_E__", "#if")
res = res.replace("__I_F_D_E_F_D_I_R_E_C_T_I_V_E__", "#ifdef")
res = res.replace("__I_F_N_D_E_F_D_I_R_E_C_T_I_V_E__", "#ifndef")
res = res.replace("__E_N_D_I_F_D_I_R_E_C_T_I_V_E__", "#endif")



res = """
/*
Attention!
This code is a compressed (and therefore very obfuscated) version of the Cotton language interpreter
available at https://github.com/lis05/Cotton

The code of the actual program is at the very bottom of this file, in function main.

The purpose of this code is to test how Cotton performs on platforms 
where you can only compile and run a singular file, like AtCoder and Codeforces.

Please do not disqualify this code!
*/\n""" + res

with open("glued.cpp", "w") as fd:
    fd.write(res)