#include "FvecsReader.h"
#include <cassert>

const char *BVF = "./sample-data/gist_query.fvecs";
const unsigned PRINT_MOST = 10;

int main(int argc, char **argv) {
  if (!(argc == 1 || argc == 2)) {
    fprintf(stderr, "Usage: %s [fvecs filename]\n\n", argv[0]);
    return EXIT_FAILURE;
  }
  std::string filename = BVF;
  if (argc == 2)
    filename = argv[1];

  FvecsReader reader(filename.c_str());
  printf("# of dim: %u, # of points: %lu\n\n", reader.pointDimension(),
         reader.numPoints());

  auto firstPoint = reader.read(1);
  auto secondPoint = reader.read(1);

  for (const auto &cor : firstPoint) {
    printf("%f ", cor);
  }
  printf("\n");
  for (const auto &cor : secondPoint) {
    printf("%f ", cor);
  }
  printf("\n");
  printf("\n");

  auto batch = reader.read(0, 2);
  int cnt = 0;
  for (const auto &cor : batch) {
    printf("%f ", cor);
    if ((++cnt) % reader.pointDimension() == 0)
      printf("\n");
  }
  printf("\n");

  reader.read(0, 1);
  auto failed = reader.read(1001, 1002);
  assert(failed.empty());
  auto tmp = reader.read(1); // should be the second point
  printf("\n");
  cnt = 0;
  for (const auto &cor : tmp) {
    printf("%f ", cor);
    if ((++cnt) % reader.pointDimension() == 0)
      printf("\n");
  }

  auto shouldresize = reader.read(999, 9990);
  assert(shouldresize.size() == reader.pointDimension());

  reader.rewind();
  auto again = reader.read(1);
  cnt = 0;
  printf("\n");
  for (const auto &cor : again) {
    printf("%f ", cor);
    if ((++cnt) % reader.pointDimension() == 0)
      printf("\n");
  }

  reader.read(998, 999);

  auto last = reader.read(100);
  assert(last.size() == reader.pointDimension());
  return EXIT_SUCCESS;
}
