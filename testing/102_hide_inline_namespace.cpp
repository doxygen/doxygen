// objective: test collapse of inline namespace
// check: 102__hide__inline__namespace_8cpp.xml
// config: HIDE_INLINE_NAMESPACE = YES
// config: EXTRACT_ALL = YES

namespace foo {

inline namespace v1 {

struct Size
{
    int width;
    int height;
};

namespace cv {

struct Image
{
    int height;
    int width;
    unsigned char* data;
};

} // namespace cv

namespace nn {

struct Tensor
{
    int width;
    int height;
    int channel;
    float* data;
};

inline namespace v1 {

struct ConvParam
{
    int pad;
    int stride;
    int kernel_len;
};

} // namespace v1

} // namespace nn

} // namespace v1

} // namespace foo

