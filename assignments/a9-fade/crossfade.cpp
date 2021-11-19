#include "atk/toolkit.h"
#include "atkui/framework.h"
#include "atkui/skeleton_drawer.h"
#include <stdio.h>
#include <string.h>

using namespace atk;
using namespace atkui;
using namespace glm;

class ACrossfadeViewer : public atkui::Framework
{
public:
  ACrossfadeViewer() : atkui::Framework(atkui::Perspective) {}
  virtual ~ACrossfadeViewer() {}

  void loadMotion1(const std::string &filename)
  {
    BVHReader reader;
    reader.load(filename, skeleton_, motion1_);
  }

  void loadMotion2(const std::string &filename)
  {
    BVHReader reader;
    reader.load(filename, skeleton_, motion2_);
  }

  void crossfade(int numBlendFrames)
  {
    assert(motion1_.getNumKeys() > 0);
    assert(motion2_.getNumKeys() > 0);
    assert(numBlendFrames > 0);
    assert(numBlendFrames <= motion1_.getNumKeys());
    assert(numBlendFrames <= motion2_.getNumKeys());

    int start1 = motion1_.getNumKeys() - numBlendFrames;
    int start2 = 0;

    
    crossfade(motion1_, motion2_, start1, start2, numBlendFrames);
  }

  void crossfade(const Motion& m1, Motion& m2, int start1, int start2, int numBlendFrames) {
      for (int i = 0; i < start1; i++) {
          blend_.appendKey(m1.getKey(i));
      }

      float alpha = 0;
      Pose first = m1.getKey(start1);
      align(first, m2);

      for (int i = 0; i < numBlendFrames; i++) {
          Pose p1 = m1.getKey(start1 + i);
          Pose p2 = m2.getKey(start2 + i);

          blend_.appendKey(Pose::Lerp(p1, p2, alpha+(float)i/(float)(numBlendFrames-1)));
      }

      for (int i = numBlendFrames; i < m2.getNumKeys(); i++) {
          blend_.appendKey(m2.getKey(i));
      }

  }

  void align(const Pose& p1, Motion& m2) {
     
      vec3 posOffset = vec3(p1.rootPos.x, 0, p1.rootPos.z) - vec3(m2.getKey(0).rootPos.x, 0, m2.getKey(0).rootPos.z);


      quat rotOffset = p1.jointRots[0] * inverse(m2.getKey(0).jointRots[0]);

      rotOffset.x = 0;
      rotOffset.z = 0;

      for (int i = 0; i < m2.getNumKeys(); i++) {
          Pose root = m2.getKey(i);
          root.rootPos += posOffset;
          root.jointRots[0] = rotOffset * root.jointRots[0];
          m2.editKey(i, root);
      }

  }


  void save(const std::string &filename)
  {
    BVHWriter writer;
    writer.save(filename, skeleton_, blend_);
  }

  void scene()
  {
    blend_.update(skeleton_, elapsedTime()*0.25f);
    drawer_.draw(skeleton_, *this);
  }

private:
  Skeleton skeleton_;
  SkeletonDrawer drawer_;
  Motion motion1_;
  Motion motion2_;
  Motion blend_;
};

std::string PruneName(const std::string &name)
{
  int idx1 = name.find_last_of("/");
  int idx2 = name.find_last_of(".");
  return name.substr(idx1 + 1, idx2 - idx1 - 1);
}

int main(int argc, char **argv)
{
  std::string motion1 = "../motions/Beta/walking.bvh";
  std::string motion2 = "../motions/Beta/jump.bvh";
  int numFrames = 10;

  try
  {
    for (int i = 0; i < argc; i++)
    {
      if (strncmp(argv[i], "-m1", 3) == 0)
        motion1 = argv[i + 1];
      else if (strncmp(argv[i], "-m2", 3) == 0)
        motion2 = argv[i + 1];
      else if (strncmp(argv[i], "-nframes", 7) == 0)
        numFrames = std::stoi(argv[i + 1]);
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "usage: crossfade -m1 <path> -m2 <path> -nframes <num frames>\n";
  }

  ACrossfadeViewer viewer;
  viewer.loadMotion1(motion1);
  viewer.loadMotion2(motion2);
  viewer.crossfade(numFrames);

  std::string saveName = "blend-" + PruneName(motion1) + PruneName(motion2);
  saveName += "-" + std::to_string(numFrames) + ".bvh";
  viewer.save(saveName);

  viewer.run();
  return 0;
}
