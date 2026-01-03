
#include <QCoreApplication>

import photoDeltaFetchingBenchmarks;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    BM_get_photo_delta();
    BM_fetch_photo_delta();
}
