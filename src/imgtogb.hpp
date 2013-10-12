#ifndef __IMGTOGB_HPP
#define __IMGTOGB_HPP

#include <string>

void produceSprites(const Image& image, bool size8x16, const std::string& name, std::ostream& os);
void produceTilemap(const Image& image, int offset, const std::string& name, std::ostream& os);
std::string getName(const std::string& path);

#endif
