#include "../include/pdf-generator.h"

#include <hpdf.h>
#include <stdexcept>
#include <string>

std::string PDFGenerator::generate(
    int jobId,
    const std::string& title,
    const std::string& content
) {
    HPDF_Doc pdf = HPDF_New(nullptr, nullptr);

    if (!pdf) {
        throw std::runtime_error("Failed to create PDF");
    }

    std::string outputPath =
        "output/report_" + std::to_string(jobId) + ".pdf";

    try {
        HPDF_Page page = HPDF_AddPage(pdf);

        HPDF_Page_SetWidth(page, 595);
        HPDF_Page_SetHeight(page, 842);

        HPDF_Font font = HPDF_GetFont(
            pdf, "Helvetica", nullptr
        );

        HPDF_Page_SetFontAndSize(page, font, 16);

        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, 50, 780, title.c_str());

        HPDF_Page_SetFontAndSize(page, font, 11);
        HPDF_Page_TextOut(page, 50, 740, content.c_str());

        HPDF_Page_EndText(page);

        if (HPDF_SaveToFile(pdf, outputPath.c_str()) != HPDF_OK) {
            throw std::runtime_error("Failed to save PDF");
        }

        HPDF_Free(pdf);
        return outputPath;
    }
    catch (...) {
        HPDF_Free(pdf);
        throw;
    }
}