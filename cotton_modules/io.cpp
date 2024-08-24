#include "../cotton_lib/src/cotton_lib/api.h"
#include <cstring>
using namespace Cotton;

class FileInstance: public Instance {
public:
    FILE       *fd;
    bool        is_open;
    bool        error;
    std::string error_message;

    FileInstance(Runtime *rt)
        : Instance(rt, sizeof(FileInstance)) {
        ProfilerCAPTURE();
        this->fd            = nullptr;
        this->is_open       = false;
        this->error         = false;
        this->error_message = "";
    }

    void close() {
        if (this->is_open && this->fd != nullptr) {
            fclose(fd);
        }
        this->is_open = false;
        this->fd      = nullptr;
    }

    ~FileInstance() {
        ProfilerCAPTURE();
        this->close();
    }

    Instance *copy(Runtime *rt) {
        ProfilerCAPTURE();
        return this;    // no copy nuh uh
    }

    size_t getSize() {
        ProfilerCAPTURE();
        return sizeof(FileInstance);
    }

    std::string userRepr(Runtime *rt) {
        ProfilerCAPTURE();
        return "File";
    }
};

class FileType: public Type {
public:
    size_t getInstanceSize() {
        return sizeof(FileInstance);
    }

    FileType(Runtime *rt)
        : Type(rt) {
        ProfilerCAPTURE();
    }

    ~FileType() = default;

    Object *create(Runtime *rt) {
        ProfilerCAPTURE();
        Instance *ins = new FileInstance(rt);
        Object   *obj = new Object(true, ins, this, rt);
        return obj;
    }

    std::string userRepr(Runtime *rt) {
        return "File";
    }

    Object *copy(Object *obj, Runtime *rt) {
        ProfilerCAPTURE();
        rt->verifyIsValidObject(obj);
        if (obj->instance == nullptr) {
            return new Object(false, nullptr, this, rt);
        }
        auto ins = obj->instance->copy(rt);
        auto res = new Object(true, ins, this, rt);
        return res;
    }
};

static Type *file_type;

static Object *fileCloseMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    auto file = icast(self->instance, FileInstance);
    file->close();

    return self;
}

static Object *fileWriteMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 1);
    auto self = args[0];
    auto arg  = args[1];
    rt->verifyIsInstanceObject(arg, rt->builtin_types.string);

    auto &data = getStringDataFast(arg);

    auto file = icast(self->instance, FileInstance);
    if (file->fd == nullptr || !file->is_open || file->error) {
        file->error_message = "Failed to write data";
        file->error         = true;
        return self;
    }

    int done = fwrite(data.c_str(), data.size(), 1, file->fd);
    if (done != 1 || ferror(file->fd)) {
        file->error_message = "Failed to write data";
        file->error         = true;
    }

    return self;
}

static Object *fileReadMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    auto file = icast(self->instance, FileInstance);
    if (file->fd == nullptr || !file->is_open || file->error) {
        file->error_message = "Failed to read data";
        file->error         = true;
        return Builtin::makeStringInstanceObject("", rt);
    }
    const static int BUFF_SIZE = 4096;
    static char      buff[BUFF_SIZE + 5];
    std::string      res;

    while (true) {
        memset(buff, '\0', sizeof(buff));
        auto cnt = fread(buff, BUFF_SIZE, 1, file->fd);
        if (ferror(file->fd)) {
            file->error_message = "Failed to read data";
            file->error         = true;
            return Builtin::makeStringInstanceObject("", rt);
        }
        res += std::string(buff);

        if (cnt != 1) {
            break;
        }
    }

    return Builtin::makeStringInstanceObject(res, rt);
}

static Object *fileErrorMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    auto file = icast(self->instance, FileInstance);
    return rt->protectedBoolean(file->error);
}

static Object *fileErrormessageMethod(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 0);
    auto self = args[0];

    auto file = icast(self->instance, FileInstance);
    return Builtin::makeStringInstanceObject(file->error_message, rt);
}

static Object *module_open(const std::vector<Object *> &args, Runtime *rt, bool execution_result_matters) {
    rt->verifyExactArgsAmountMethod(args, 2);
    auto arg1 = args[1];
    auto arg2 = args[2];
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, MethodArgCtx(0));
    rt->verifyIsInstanceObject(arg1, rt->builtin_types.string, MethodArgCtx(1));

    auto &filename = getStringDataFast(arg1);
    auto &mode     = getStringDataFast(arg2);

    auto fd    = fopen(filename.c_str(), mode.c_str());
    auto res   = rt->make(file_type, rt->INSTANCE_OBJECT);
    auto filei = icast(res->instance, FileInstance);
    if (fd == nullptr) {
        filei->fd            = NULL;
        filei->is_open       = false;
        filei->error         = true;
        filei->error_message = std::strerror(errno);
    }
    else {
        filei->fd            = fd;
        filei->is_open       = true;
        filei->error         = false;
        filei->error_message = "";
    }

    return res;
}

extern "C" Object *library_load_point(Runtime *rt) {
    file_type = new FileType(rt);
    file_type->addMethod(rt->nmgr->getId("close"), Builtin::makeFunctionInstanceObject(true, fileCloseMethod, nullptr, rt));
    file_type->addMethod(rt->nmgr->getId("write"), Builtin::makeFunctionInstanceObject(true, fileWriteMethod, nullptr, rt));
    file_type->addMethod(rt->nmgr->getId("read"), Builtin::makeFunctionInstanceObject(true, fileReadMethod, nullptr, rt));
    file_type->addMethod(rt->nmgr->getId("error"), Builtin::makeFunctionInstanceObject(true, fileErrorMethod, nullptr, rt));
    file_type->addMethod(rt->nmgr->getId("errormessage"), Builtin::makeFunctionInstanceObject(true, fileErrormessageMethod, nullptr, rt));

    auto record = Builtin::makeRecordType(rt->nmgr->getId("IO"), rt);
    record->addMethod(rt->nmgr->getId("open"), Builtin::makeFunctionInstanceObject(true, module_open, nullptr, rt));
    return rt->make(record, Runtime::INSTANCE_OBJECT);
}
