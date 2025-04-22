#include "Error.h"

Error::Error() : code(ErrorCode::Success), text("") {}

void Error::SetError(ErrorCode code, const std::string& text) {
    this->code = code;
    this->text = text;
}

void Error::AppendContext(const std::string& context) {
    if (code != ErrorCode::Success) {
        text += "; " + context;
    }
}

void Error::Clear() {
    code = ErrorCode::Success;
    text.clear();
}

int Error::GetCode() const {
    return static_cast<int>(code);
}

const std::string& Error::GetText() const {
    return text;
}

bool Error::IsSet() const {
    return code != ErrorCode::Success;
}
