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

#pragma once

#include "../front/parser.h"
#include "../util.h"
#include "nameid.h"
#include "object.h"
#include <string>
#include <vector>

namespace Cotton {
class Scope;
class GC;
class Object;
class Instance;
class Type;
class ErrorManager;
class Token;
class ExprNode;
class StmtNode;
class GCStrategy;

namespace Builtin {
    class NothingType;
    class BooleanType;
    class FunctionType;
    class IntegerType;
    class RealType;
    class CharacterType;
    class StringType;
    class ArrayType;
}    // namespace Builtin

/// @brief Class that is responsible for actual execution of the Cotton language.
class Runtime {
    friend class GC;

private:
    HashTable<NameId, Object *> globals;

    HashTable<Type *, Object *> registered_type_objects;
    ErrorManager               *error_manager;
    GC                         *gc;

    HashTable<NameId, Object *> readonly_literals;

    Object *protected_nothing;
    Object *protected_true;
    Object *protected_false;

    Scope *scope;

    uint8_t execution_flags;

    enum ExecutionFlags { NONE = 0, CONTINUE = 1, BREAK = 2, RETURN = 4, DIRECT_PASS = 8 };

public:
    /// @brief Class that is used as the context when signalling errors
    class ErrorContext {
    public:
        /// @brief TextArea that covers the entire context. Is expected to be always valid.
        TextArea area;

        /// @brief Sub areas of the current context. Usually, each sub area represents an argument in the context.
        /// When function is called, then sub_areas[0] will be the area of the function, and sub_areas[i] will be
        /// the area of the ith argument
        std::vector<TextArea> sub_areas;
    };

    /// @brief These are used to select the context part when signaling errors.
    enum ContextId { AREA_CTX = -1, SUB0_CTX = 0, SUB1_CTX = 1, SUB2_CTX = 2, SUB3_CTX = 3 };

    /// @brief Options which indicate which object make() should produce: instance object or type object
    enum ObjectOptions { INSTANCE_OBJECT, TYPE_OBJECT };

    /// @brief NamesManager of the runtime.
    NamesManager *const nmgr;

    /// @brief Class storing all builtin types.
    class {
    public:
        Builtin::FunctionType      *function;
        Builtin::NothingType       *nothing;
        Builtin::BooleanType       *boolean;
        Builtin::IntegerType       *integer;
        Builtin::RealType          *real;
        Builtin::CharacterType     *character;
        Builtin::StringType        *string;
        Builtin::ArrayType         *array;
    } builtin_types;

private:
    std::vector<ErrorContext> error_contexts;

public:
    /**
     * @brief Construct a new Runtime object.
     *
     * @param gc_strategy Garbage collector stategy. Must be valid.
     * @param error_manager Error manager. Must be valid.
     * @param nmgr Names manager. Must be valid.
     */
    Runtime(GCStrategy *gc_strategy, ErrorManager *error_manager, NamesManager *nmgr);

    /// @brief Destruct the runtime. // TODO: add proper destruction
    ~Runtime() = default;

    /**
     * @brief Returns whether a global variable with the given id exists.
     *
     * @param id Id of the global variable.
     * @return `true` if a global variable with id exists, `false` otherwise.
     */
    bool checkGlobal(NameId id);

    /**
     * @brief Returns the global variable associated with the given id. Signals an error if no such variable
     * exists.
     *
     * @param id The id of the global variable. Must be valid.
     * @return Global variable associated with the given id.
     */
    Object *getGlobal(NameId id);

    /**
     * @brief Sets the global variable with the given id to obj.
     *
     * @param id The id of the global variable.
     * @param obj Object. Must be valid.
     */
    void setGlobal(NameId id, Object *obj);

    /**
     * @brief Removes the global variable with the given id. If no such variable exists - nothing happens.
     *
     * @param id The id of the global variable.
     */
    void removeGlobal(NameId id);

    /**
     * @brief Registers `obj` as a type object of the given `type`.
     *
     * @param type Type with which `obj` will be associated. Must be valid.
     * @param obj Object that will be registered. Must be a type object of the given type.
     */
    void registerTypeObject(Type *type, Object *obj);

    /**
     * @brief Returns the type object registered under the given type. If no such object exists - returns
     * Runtime::protectedNothing().
     *
     * @param type The type of the requested object. Must be valid.
     * @return Object registered with the given type, or Runtime::protectedNothing() if no such object exists.
     */
    Object *getTypeObject(Type *type);

    /**
     * @brief Returns a protected from assignment instance object of type Nothing.
     *
     * @return Protected from assignment instance object of type Nothing.
     */
    Object *protectedNothing();

    /**
     * @brief Returns a protected from assignment instance object of type Boolean depending on the provided value.
     *
     * @param val Value of the returned object.
     * @return if `val` is `true`, then returns a protected from assignment instace object of type Boolean with
     * value `true`, otherwise with value `false`
     */
    Object *protectedBoolean(bool val);

    /**
     * @brief Creates a new scope frame.
     *
     * @param can_access_prev_scope if `true`, the new scope frame will be able to access its parent scope frame.
     * Otherwise it won't.
     */
    void newScopeFrame(bool can_access_prev_scope = true);

    /// @brief Pops the topmost scope frame.
    void popScopeFrame();

    /// @brief Creates a new empty error context.
    void newContext();

    /// @brief Pops the topmost error context.
    void popContext();

    /// @brief Returns the current error context.
    ErrorContext &getContext();

    /**
     * @brief Returns the selected text area from the current context.
     *
     * @param ctx_id Id of the selected part of the context.
     * @return TextArea
     */
    TextArea &getTextArea(ContextId ctx_id);

    /**
     * @brief Signals an error.
     *
     * @param message Message to be desplayed.
     * @param ta TextArea of the error place.
     */
    [[noreturn]]
    void signalError(const std::string &message, const TextArea &ta);

    /**
     * @brief Executes the given node.
     *
     * @param node Node to be executed. Must be valid.
     * @param execution_result_matters If `false`, certain optimizations may happen.
     * @return The result of the execution. Is not guaranteed to return anything if `execution_result_matters` is
     * `false`
     */
    ///@{
    Object *execute(ExprNode *node, bool execution_result_matters);
    Object *execute(FuncDefNode *node, bool execution_result_matters);
    Object *execute(TypeDefNode *node, bool execution_result_matters);
    Object *execute(OperatorNode *node, bool execution_result_matters);
    Object *execute(AtomNode *node, bool execution_result_matters);
    Object *execute(ParExprNode *node, bool execution_result_matters);
    Object *execute(StmtNode *node, bool execution_result_matters);
    Object *execute(WhileStmtNode *node, bool execution_result_matters);
    Object *execute(ForStmtNode *node, bool execution_result_matters);
    Object *execute(IfStmtNode *node, bool execution_result_matters);
    Object *execute(ReturnStmtNode *node, bool execution_result_matters);
    Object *execute(BlockStmtNode *node, bool execution_result_matters);

    ///@}

    /**
     * @brief Makes a new object of the given type. If object opt is INSTANCE_OBJECT, then the created object will
     * be an instance object. Otherwise, it will be a type object. If creation of the object fails - an error will
     * be signaled. The returned object will be marked with "single use".
     *
     * @param type The type of the object to create. Must be valid.
     * @param object_opt If INSTANCE_OBJECT, then an instance object will be created. Otherwise, creates a type
     * object.
     * @return The created object.
     */
    Object *make(Type *type, ObjectOptions object_opt);

    /**
     * @brief Returns a copy of the object. The returned object will be marked with "single use".
     *
     * @param obj The object to copy. Must be valid.
     * @return The copy of the given object.
     */
    Object *copy(Object *obj);

    /**
     * @brief Runs unary operator with the given id on the object.
     *
     * @param id Id of the unary operator. Must correspond to an unary operator.
     * @param obj Argument of the unary operator. Must be valid.
     * @param execution_result_matters if `false`, the returned object is not guaranteed to be valid. However,
     * certain optimizations will be run.
     *
     * @return Result of the operator. May not be valid if `execution_result_matters` is `false`.
     */
    Object *runOperator(OperatorNode::OperatorId id, Object *obj, bool execution_result_matters);

    /**
     * @brief Runs binary operator with the given id on the object.
     *
     * @param id Id of the binary operator. Must correspond to an binary operator.
     * @param obj First argument of the binary operator. Must be valid.
     * @param arg Second argument of the binary operator. Must be valid.
     * @param execution_result_matters if `false`, the returned object is not guaranteed to be valid. However,
     * certain optimizations will be run.
     *
     * @return Result of the operator. May not be valid if `execution_result_matters` is `false`.
     */
    Object *runOperator(OperatorNode::OperatorId id, Object *obj, Object *arg, bool execution_result_matters);

    /**
     * @brief Runs a n-ary operator with the given id on the object.
     *
     * @param id Id of the binary operator. Must correspond to a n-ary operator.
     * @param obj First argument of the operator. Must be valid.
     * @param args Rest of the arguments of the operator. Each argument must be valid.
     * @param execution_result_matters if `false`, the returned object is not guaranteed to be valid. However,
     * certain optimizations will be run.
     *
     * @return Result of the operator. May not be valid if `execution_result_matters` is `false`.
     */
    Object *runOperator(OperatorNode::OperatorId     id,
                        Object                      *obj,
                        const std::vector<Object *> &args,
                        bool                         execution_result_matters);

    /**
     * @brief Runs method with the given id. Signals an error if no such method exists.
     *
     * @param id Id of the method.
     * @param obj Object on which the method will be run. Must be valid.
     * @param args Arguments of the method. Each of the arguments must be valid.
     * @param execution_result_matters if `false`, the returned object is not guaranteed to be valid. However,
     * certain optimizations will be run.
     *
     * @return Result of the method. May not be valid if `execution_result_matters` is `false`.
     */
    Object *runMethod(NameId id, Object *obj, const std::vector<Object *> &args, bool execution_result_matters);

    /**
     * @brief Checks whether the provided object is valid.
     *
     * @param obj The object to check.
     * @return `true` if the provided object is valid, `false` otherwise.
     */
    bool isValidObject(Object *obj);

    /**
     * @brief Checks whether the provided object is a valid type object of the given type. If `type` = nullptr,
     * then checks if the provided object is a type object of any type.
     *
     *
     * @param obj The object to check.
     * @param type Type to check. nullptr means that the check will be done for any type.
     * @return `true` if the provided object is a type object of the given type, `false` otherwise.
     */
    bool isTypeObject(Object *obj, Type *type = nullptr);

    /**
     * @brief Checks whether the provided object is a valid instance object of the given type. If `type` = nullptr,
     * then checks if the provided object is an instance object of any type.
     *
     *
     * @param obj The object to check.
     * @param type Type to check. nullptr means that the check will be done for any type.
     * @return `true` if the provided object is an onstance object of the given type, `false` otherwise.
     */
    bool isInstanceObject(Object *obj, Type *type = nullptr);

    /**
     * @brief Checks whether the provided object is a valid object of the given type.
     *
     * @param obj The object to check.
     * @param type Type to check. Must be non-nullptr.
     * @return `true` if the provided object is an object of the given type, `false` otherwise.
     */
    bool isOfType(Object *obj, Type *type);

    /**
     * @brief Signals an error if `isValidObject(obj, ctx_id)` returns `false`
     *
     * @param obj See `isValidObject`
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyIsValidObject(Object *obj, ContextId ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Signals an error if `isTypeObject(obj, type, ctx_id)` returns `false`
     *
     * @param obj See `isTypeObject`
     * @param type See `isTypeObject`
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyIsTypeObject(Object *obj, Type *type, ContextId ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Signals an error if `isInstanceObject(obj, type, ctx_id)` returns `false`
     *
     * @param obj See `isInstanceObject`
     * @param type See `isInstanceObject`
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyIsInstanceObject(Object *obj, Type *type, ContextId ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Signals an error if `isOfType(obj, type, ctx_id)` returns `false`
     *
     * @param obj See `isOfType`
     * @param type See `isOfType`
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyIsOfType(Object *obj, Type *type, ContextId ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Signals an error if the provided list of arguments contains less elements than `amount`.
     * Use this to check the amount of arguments passed to a function. For a method, use
     * `verifyMinArgsAmountMethod`
     *
     * @param args List of arguments to check.
     * @param amount The minimal amount of arguments.
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyMinArgsAmountFunc(const std::vector<Object *> &args,
                                 int64_t                      amount,
                                 ContextId                    ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Signals an error if the provided list of arguments contains different amount of elements than
     * `amount`. Use this to check the amount of arguments passed to a function. For a method, use
     * `verifyExactArgsAmountMethod`
     *
     * @param args List of arguments to check.
     * @param amount The exact amount of arguments.
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyExactArgsAmountFunc(const std::vector<Object *> &args,
                                   int64_t                      amount,
                                   ContextId                    ctx_id = ContextId::AREA_CTX);
    /**
     * @brief Signals an error if the provided list of arguments contains less elements than `amount`. Since the
     * first argument is always present, it is not counted in the total number of arguments. Therefore, this
     * function actually checks if the provided list has amount + 1 arguments (at least).
     * Use this to check the amount of arguments passed to a mathod. For a function, use
     * `verifyMinArgsAmountFunc`
     *
     * @param args List of arguments to check.
     * @param amount The minimal amount of arguments.
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyMinArgsAmountMethod(const std::vector<Object *> &args,
                                   int64_t                      amount,
                                   ContextId                    ctx_id = ContextId::AREA_CTX);
    /**
     * @brief Signals an error if the provided list of arguments contains different number of elements than
     * `amount`. Since the first argument is always present, it is not counted in the total number of arguments.
     * Therefore, this function actually checks if the provided list has amount + 1 arguments. Use this to check
     * the amount of arguments passed to a mathod. For a function, use `verifyExactArgsAmountFunc`
     *
     * @param args List of arguments to check.
     * @param amount The minimal amount of arguments.
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyExactArgsAmountMethod(const std::vector<Object *> &args,
                                     int64_t                      amount,
                                     ContextId                    ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Signals an error if the provided object doesn't have method with the given id.
     *
     * @param obj Object to check.
     * @param id Id of the method.
     * @param ctx_id Id of the context part which will be included in the error message.
     */
    void verifyHasMethod(Object *obj, NameId id, ContextId ctx_id = ContextId::AREA_CTX);

    /**
     * @brief Returns the current garbage collector.
     *
     * @return GC*
     */
    GC *getGC();

    /**
     * @brief Returns the current error manager.
     *
     * @return ErrorManager*
     */
    ErrorManager *getErrorManager();

    /**
     * @brief Returns the current scope.
     *
     * @return Scope*
     */
    Scope *getScope();

    /// @brief Sets the runtime's execution flag to NONE. This means that execute() will end normally.
    /// This is called when execute() is about to end normally()
    void clearExecFlags();

    /// @brief Sets the runtime's execution flag to CONTINUE. This means that execute() will end with `continue`
    /// keyword. This is called when execute() has executed a `continue` instruction that needs to be passed
    /// upwards.
    void setExecFlagCONTINUE();

    /// @brief Sets the runtime's execution flag to BREAK. This means that execute() will end with `break` flag.
    /// This is called when execute() has executed a `break` instruction that needs to be passed upwards.
    void setExecFlagBREAK();

    /// @brief Sets the runtime's execution flag to RETURN. This means that execute() will end with `return` flag.
    /// This is called when execute() has executed a `return` instruction that needs to be passed upwards.
    void setExecFlagRETURN();

    /// @brief Sets the runtime's execution flag to DIRECT_PASS. This means that execute() will end with
    /// `direct_pass` flag. This is called when execute() has executed a `direct pass` instruction that needs to be
    /// passed upwards.
    void setExecFlagDIRECT_PASS();

    /// @brief Returns whether the execution flag contains NONE subflag.
    bool isExecFlagNONE();

    /// @brief Returns whether the execution flag contains CONTINUE subflag.
    bool isExecFlagCONTINUE();

    /// @brief Returns whether the execution flag contains BREAK subflag.
    bool isExecFlagBREAK();

    /// @brief Returns whether the execution flag contains RETURN subflag.
    bool isExecFlagRETURN();

    /// @brief Returns whether the execution flag contains DIRECT_PASS subflag.
    bool isExecFlagDIRECT_PASS();
};

/// @brief Commonly used methods' ids
namespace MagicMethods {
    /**
     * @brief Returns the nameid of the method called __make__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__make__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __copy__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__copy__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __bool__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__bool__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __char__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__char__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __int__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__int__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __real__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__real__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __string__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__string__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __repr__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__repr__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __read__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
    NameId mm__read__(Runtime *rt);
    /**
     * @brief Returns the nameid of the method called __get_iterator__.
     *
     * @param rt The runtime. Must be valid.
     * @return NameId of the method.
     */
}    // namespace MagicMethods

/// @brief Every library must implement this. It is run when the library gets loaded.
typedef Object *(*LibraryLoadPoint)(Runtime *rt);

/// @brief casts instance to another instance
#define icast(ins, type) ((type *)ins)
}    // namespace Cotton
