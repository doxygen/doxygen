#ifndef MSCGEN_API_H
#define MSCGEN_API_H

#ifdef __cplusplus
extern "C" {
#endif

#define MSCGEN_OK                   ( 0)
#define MSCGEN_FILE_ERROR           (-1)
#define MSCGEN_INPUT_FORMAT_ERROR   (-2)
#define MSCGEN_OUTPUT_CONTEXT_ERROR (-3)

/** The supported image formats */
typedef enum
{
  mscgen_format_png, /**< PNG bitmap image file */
  mscgen_format_eps, /**< Encapsulated PostScript file */
  mscgen_format_svg, /**< Scalable Vector Graphics file */
  mscgen_format_pngmap, /**< Image map for a bitmap file */
  mscgen_format_svgmap  /**< Image map for a SVG file */
} mscgen_format_t;

/** generate an image file for a given .msc file.
 *  @param inputFile the name of the MSC file to process.
 *  @param outputFile the name of the image file to generate.
 *  @param format the format of the image file to generate.
 *  @return 0 on success, a non zero error code on failure.
 */
int mscgen_generate(const char *inputFile,
                    const char *outputFile,
                    mscgen_format_t format
                   );

/** Translates the error code returned by mscgen_generate into a string */
const char *mscgen_error2str(int code);

#ifdef __cplusplus
}
#endif

#endif // MSCGEN_API_H
