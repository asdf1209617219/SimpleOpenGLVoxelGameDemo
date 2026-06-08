#include <dme/block/cube_face.h>
#include <dme/block/block_data.h>
namespace dme::block {
    static Face3 rotateForXYZFace3[36] = {
        Face3c::Front , Face3c::Bottom , Face3c::Back  , Face3c::Top    , Face3c::Left   , Face3c::Right  , //以x轴逆时针各个面下标
        Face3c::Front , Face3c::Top    , Face3c::Back  , Face3c::Bottom , Face3c::Right  , Face3c::Left   , //以x轴顺时针各个面下标
        Face3c::Top  , Face3c::Left   , Face3c::Bottom , Face3c::Left   , Face3c::Back  , Face3c::Front   , //以y轴逆时针各个面下标
        Face3c::Top  , Face3c::Right  , Face3c::Bottom , Face3c::Right  , Face3c::Front   , Face3c::Back  , //以y轴顺时针各个面下标
        Face3c::Right, Face3c::Back  , Face3c::Left   , Face3c::Front   , Face3c::Bottom , Face3c::Top    , //以z轴逆时针各个面下标
        Face3c::Right, Face3c::Front   , Face3c::Left   , Face3c::Back  , Face3c::Top    , Face3c::Bottom , //以z轴顺时针各个面下标
    };
    static uint xyzFace3ToIndex[36] = {
          5, 4, 0, 2, 3, 1,
          4, 5, 0, 1, 3, 2,
          3, 1, 5, 4, 0, 2,
          3, 2, 4, 5, 0, 1,
          0, 2, 3, 1, 5, 4,
          0, 1, 3, 2, 4, 5,
    };
    //根据atFace所在的轴顺时针旋转（如果处于负轴，则表示逆时针旋转），获取旋转前对应的面（即和旋转方向相反）
    inline static Face3 rotateXYZGetStartFaceIndex(Face3 atFace, FRotate rotate, Face3 face) {
        if (atFace == face || atFace == face.getInvert()) {
            return face;
        }
        return rotateForXYZFace3[
            (xyzFace3ToIndex[atFace * 6 + face] + rotate) & 0b11
        ];
    }

    //将atFace所在的面，向着该面的x方向或y方向（即右和上）旋转rotate次，获得对应的纹理坐标需要的旋转次数
    //inline static FRotate rotateFaceGetFaceRotate(Face3 atFace, FRotate rotate, bool isY) {
    //    return rotateFaceRotateTable[atFace * 8 + rotate + (isY ? 4 : 0)];
    //}
    static FRotate rotateFaceRotateTable[36] = {
        FRotatec::N, FRotatec::N, FRotatec::N, FRotatec::CCW, FRotatec::CS, FRotatec::CW,
        FRotatec::N, FRotatec::N, FRotatec::N, FRotatec::CW, FRotatec::CS, FRotatec::CCW,
        FRotatec::N, FRotatec::N, FRotatec::N, FRotatec::CS, FRotatec::CS, FRotatec::CS,
        FRotatec::N, FRotatec::N, FRotatec::N, FRotatec::N, FRotatec::CS, FRotatec::N,
        FRotatec::CW, FRotatec::CS, FRotatec::CCW, FRotatec::CS, FRotatec::N, FRotatec::N,
        FRotatec::CCW, FRotatec::CS, FRotatec::CW, FRotatec::N, FRotatec::N, FRotatec::CS,
    };

    //查找用的表，表示每个面旋转后对应的原来的面以及纹理坐标需要的旋转次数
    static Face3Rotate cubeFace3RotateTable[4 * 6 * 6] = {
        //右面在x+
        Face3Rotate(Face3c::Right, FRotatec::N),  Face3Rotate(Face3c::Left, FRotatec::N),            Face3Rotate(Face3c::Front, FRotatec::N),              Face3Rotate(Face3c::Back, FRotatec::N),           Face3Rotate(Face3c::Top, FRotatec::N),             Face3Rotate(Face3c::Bottom, FRotatec::N), //旋转0次
        Face3Rotate(Face3c::Right, FRotatec::CCW),Face3Rotate(Face3c::Left, FRotatec::CW),       Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Top, FRotatec::N),             Face3Rotate(Face3c::Front, FRotatec::CS), Face3Rotate(Face3c::Back, FRotatec::N), //旋转1次
        Face3Rotate(Face3c::Right, FRotatec::CS), Face3Rotate(Face3c::Left, FRotatec::CS), Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::CS), Face3Rotate(Face3c::Bottom, FRotatec::N),          Face3Rotate(Face3c::Top, FRotatec::N), //旋转2次
        Face3Rotate(Face3c::Right, FRotatec::CW), Face3Rotate(Face3c::Left, FRotatec::CCW),   Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Bottom, FRotatec::N),          Face3Rotate(Face3c::Back, FRotatec::N),           Face3Rotate(Face3c::Front, FRotatec::CS), //旋转3次
        //x-
        Face3Rotate(Face3c::Left, FRotatec::N),   Face3Rotate(Face3c::Right, FRotatec::N),            Face3Rotate(Face3c::Back, FRotatec::N),           Face3Rotate(Face3c::Front, FRotatec::N),              Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Bottom, FRotatec::CS),
        Face3Rotate(Face3c::Left, FRotatec::CW),  Face3Rotate(Face3c::Right, FRotatec::CCW),   Face3Rotate(Face3c::Top, FRotatec::N),             Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Front, FRotatec::N),              Face3Rotate(Face3c::Back, FRotatec::CS),
        Face3Rotate(Face3c::Left, FRotatec::CS),  Face3Rotate(Face3c::Right, FRotatec::CS), Face3Rotate(Face3c::Front, FRotatec::CS), Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Top, FRotatec::CS),
        Face3Rotate(Face3c::Left, FRotatec::CCW), Face3Rotate(Face3c::Right, FRotatec::CW),       Face3Rotate(Face3c::Bottom, FRotatec::N),          Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::N),
        //y+
        Face3Rotate(Face3c::Back, FRotatec::N),   Face3Rotate(Face3c::Front, FRotatec::N),             Face3Rotate(Face3c::Right, FRotatec::N),           Face3Rotate(Face3c::Left, FRotatec::N),            Face3Rotate(Face3c::Top, FRotatec::CCW),    Face3Rotate(Face3c::Bottom, FRotatec::CW),
        Face3Rotate(Face3c::Top, FRotatec::N),    Face3Rotate(Face3c::Bottom, FRotatec::CS),Face3Rotate(Face3c::Right, FRotatec::CCW),  Face3Rotate(Face3c::Left, FRotatec::CW),       Face3Rotate(Face3c::Front, FRotatec::CW),       Face3Rotate(Face3c::Back, FRotatec::CW),
        Face3Rotate(Face3c::Front, FRotatec::CS), Face3Rotate(Face3c::Back, FRotatec::CS), Face3Rotate(Face3c::Right, FRotatec::CS),Face3Rotate(Face3c::Left, FRotatec::CS), Face3Rotate(Face3c::Bottom, FRotatec::CCW), Face3Rotate(Face3c::Top, FRotatec::CW),
        Face3Rotate(Face3c::Bottom, FRotatec::N), Face3Rotate(Face3c::Top, FRotatec::CS),   Face3Rotate(Face3c::Right, FRotatec::CW),      Face3Rotate(Face3c::Left, FRotatec::CCW),   Face3Rotate(Face3c::Back, FRotatec::CCW),  Face3Rotate(Face3c::Front, FRotatec::CCW),
        //y-
        Face3Rotate(Face3c::Front, FRotatec::N),  Face3Rotate(Face3c::Back, FRotatec::N),          Face3Rotate(Face3c::Left, FRotatec::N),            Face3Rotate(Face3c::Right, FRotatec::N),           Face3Rotate(Face3c::Top, FRotatec::CW),        Face3Rotate(Face3c::Bottom, FRotatec::CCW),
        Face3Rotate(Face3c::Bottom, FRotatec::CS),Face3Rotate(Face3c::Top, FRotatec::N),            Face3Rotate(Face3c::Left, FRotatec::CW),       Face3Rotate(Face3c::Right, FRotatec::CCW),  Face3Rotate(Face3c::Front, FRotatec::CCW),   Face3Rotate(Face3c::Back, FRotatec::CCW),
        Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::CS),Face3Rotate(Face3c::Left, FRotatec::CS), Face3Rotate(Face3c::Right, FRotatec::CS),Face3Rotate(Face3c::Bottom, FRotatec::CW),     Face3Rotate(Face3c::Top, FRotatec::CCW),
        Face3Rotate(Face3c::Top, FRotatec::CS),   Face3Rotate(Face3c::Bottom, FRotatec::N),         Face3Rotate(Face3c::Left, FRotatec::CCW),   Face3Rotate(Face3c::Right, FRotatec::CW),      Face3Rotate(Face3c::Back, FRotatec::CW),      Face3Rotate(Face3c::Front, FRotatec::CW),
        //z+
        Face3Rotate(Face3c::Front, FRotatec::CW),  Face3Rotate(Face3c::Back, FRotatec::CCW),  Face3Rotate(Face3c::Top, FRotatec::CCW),    Face3Rotate(Face3c::Bottom, FRotatec::CCW), Face3Rotate(Face3c::Right, FRotatec::N),           Face3Rotate(Face3c::Left, FRotatec::CS),
        Face3Rotate(Face3c::Bottom, FRotatec::CCW),Face3Rotate(Face3c::Top, FRotatec::CCW),    Face3Rotate(Face3c::Front, FRotatec::CW),       Face3Rotate(Face3c::Back, FRotatec::CCW),  Face3Rotate(Face3c::Right, FRotatec::CCW),  Face3Rotate(Face3c::Left, FRotatec::CCW),
        Face3Rotate(Face3c::Back, FRotatec::CCW),  Face3Rotate(Face3c::Front, FRotatec::CW),       Face3Rotate(Face3c::Bottom, FRotatec::CCW), Face3Rotate(Face3c::Top, FRotatec::CCW),    Face3Rotate(Face3c::Right, FRotatec::CS),Face3Rotate(Face3c::Left, FRotatec::N),
        Face3Rotate(Face3c::Top, FRotatec::CCW),   Face3Rotate(Face3c::Bottom, FRotatec::CCW), Face3Rotate(Face3c::Back, FRotatec::CCW),  Face3Rotate(Face3c::Front, FRotatec::CW),       Face3Rotate(Face3c::Right, FRotatec::CW),      Face3Rotate(Face3c::Left, FRotatec::CW),
        //z-
        Face3Rotate(Face3c::Front, FRotatec::CCW), Face3Rotate(Face3c::Back, FRotatec::CW),    Face3Rotate(Face3c::Bottom, FRotatec::CW),     Face3Rotate(Face3c::Top, FRotatec::CCW),        Face3Rotate(Face3c::Right, FRotatec::N),           Face3Rotate(Face3c::Left, FRotatec::CS),
        Face3Rotate(Face3c::Bottom, FRotatec::CW), Face3Rotate(Face3c::Top, FRotatec::CW),   Face3Rotate(Face3c::Back, FRotatec::CCW),  Face3Rotate(Face3c::Front, FRotatec::CCW),       Face3Rotate(Face3c::Right, FRotatec::CCW),  Face3Rotate(Face3c::Left, FRotatec::CCW),
        Face3Rotate(Face3c::Back, FRotatec::CW),   Face3Rotate(Face3c::Front, FRotatec::CCW), Face3Rotate(Face3c::Top, FRotatec::CCW),    Face3Rotate(Face3c::Bottom, FRotatec::CCW),     Face3Rotate(Face3c::Right, FRotatec::CS),Face3Rotate(Face3c::Left, FRotatec::N),
        Face3Rotate(Face3c::Top, FRotatec::CW),    Face3Rotate(Face3c::Bottom, FRotatec::CW),      Face3Rotate(Face3c::Front, FRotatec::CCW),   Face3Rotate(Face3c::Back, FRotatec::CW),          Face3Rotate(Face3c::Right, FRotatec::CW),      Face3Rotate(Face3c::Left, FRotatec::CW),
    };

    //符合条件就返回1否则返回0，自动转为旋转类型的值
    inline static FRotate trueThenRotateOne(bool condition) {
        return condition ? FRotatec::CW : FRotatec::N;
    }
    //符合条件就返回输入否则返回与之对称的旋转
    inline static FRotate falseThenRotateSymmetry(bool condition, FRotate rotate) {
        return condition ? rotate : rotate.symmetry();
    }


    Face3Rotate CubeFaceToward::face3SpinToStartFace3Rotate(Face3 face, CubeFaceSpin spin) const noexcept {
        Face3Rotate result = cubeFace3RotateTable[getRightToward() * 24 + getFRotate() * 6 + face];
        result.rotate = result.rotate.directionCount(FRotatec::CW, spin.get(face));
        return result;
    }

    //查找用的表，表示每个面旋转后对应的原来的面以及纹理坐标需要的旋转次数
    static Face3Rotate cubeFaceTowardTable[4 * 6 * 6] = {
        //右面在x+
        //查找此时的x+面的被旋转次数                                     //查找此时的x-面的被旋转次数                                     //查找此时的y+面的被旋转次数                                     //查找此时的y-面的被旋转次数                                     //查找此时的z+面的被旋转次数                                     //查找此时的z-面的被旋转次数
        Face3Rotate(Face3c::Right, FRotatec::N),         Face3Rotate(Face3c::Left, FRotatec::N),          Face3Rotate(Face3c::Front, FRotatec::N),          Face3Rotate(Face3c::Back, FRotatec::N),         Face3Rotate(Face3c::Top, FRotatec::N),           Face3Rotate(Face3c::Bottom, FRotatec::N),      //旋转0次
        Face3Rotate(Face3c::Right, FRotatec::CCW),    Face3Rotate(Face3c::Left, FRotatec::CW),         Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Top, FRotatec::N),           Face3Rotate(Face3c::Front, FRotatec::CS),   Face3Rotate(Face3c::Back, FRotatec::N),       //旋转1次
        Face3Rotate(Face3c::Right, FRotatec::CS),  Face3Rotate(Face3c::Left, FRotatec::CS),   Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::CS),   Face3Rotate(Face3c::Bottom, FRotatec::N),        Face3Rotate(Face3c::Top, FRotatec::N),         //旋转2次
        Face3Rotate(Face3c::Right, FRotatec::CW),        Face3Rotate(Face3c::Left, FRotatec::CCW),     Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Bottom, FRotatec::N),        Face3Rotate(Face3c::Back, FRotatec::N),         Face3Rotate(Face3c::Front, FRotatec::CS), //旋转3次
        //右面在x-
        Face3Rotate(Face3c::Left, FRotatec::N),          Face3Rotate(Face3c::Right, FRotatec::N),         Face3Rotate(Face3c::Back, FRotatec::N),         Face3Rotate(Face3c::Front, FRotatec::N),          Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Bottom, FRotatec::CS),
        Face3Rotate(Face3c::Left, FRotatec::CW),         Face3Rotate(Face3c::Right, FRotatec::CCW),    Face3Rotate(Face3c::Top, FRotatec::N),           Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Front, FRotatec::N),          Face3Rotate(Face3c::Back, FRotatec::CS),
        Face3Rotate(Face3c::Left, FRotatec::CS),   Face3Rotate(Face3c::Right, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::CS),   Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Top, FRotatec::CS),
        Face3Rotate(Face3c::Left, FRotatec::CCW),     Face3Rotate(Face3c::Right, FRotatec::CW),        Face3Rotate(Face3c::Bottom, FRotatec::N),        Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::N),
        //右面在y+
        Face3Rotate(Face3c::Back, FRotatec::N),         Face3Rotate(Face3c::Front, FRotatec::N),          Face3Rotate(Face3c::Right, FRotatec::N),         Face3Rotate(Face3c::Left, FRotatec::N),          Face3Rotate(Face3c::Top, FRotatec::CCW),      Face3Rotate(Face3c::Bottom, FRotatec::CW),
        Face3Rotate(Face3c::Top, FRotatec::N),           Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Right, FRotatec::CCW),    Face3Rotate(Face3c::Left, FRotatec::CW),         Face3Rotate(Face3c::Front, FRotatec::CW),         Face3Rotate(Face3c::Back, FRotatec::CW),
        Face3Rotate(Face3c::Front, FRotatec::CS),   Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Right, FRotatec::CS),  Face3Rotate(Face3c::Left, FRotatec::CS),   Face3Rotate(Face3c::Bottom, FRotatec::CCW),   Face3Rotate(Face3c::Top, FRotatec::CW),
        Face3Rotate(Face3c::Bottom, FRotatec::N),        Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Right, FRotatec::CW),        Face3Rotate(Face3c::Left, FRotatec::CCW),     Face3Rotate(Face3c::Back, FRotatec::CCW),    Face3Rotate(Face3c::Front, FRotatec::CCW),
        //右面在y-
        Face3Rotate(Face3c::Front, FRotatec::N),          Face3Rotate(Face3c::Back, FRotatec::N),         Face3Rotate(Face3c::Left, FRotatec::N),          Face3Rotate(Face3c::Right, FRotatec::N),         Face3Rotate(Face3c::Top, FRotatec::CW),          Face3Rotate(Face3c::Bottom, FRotatec::CCW),
        Face3Rotate(Face3c::Bottom, FRotatec::CS), Face3Rotate(Face3c::Top, FRotatec::N),           Face3Rotate(Face3c::Left, FRotatec::CW),         Face3Rotate(Face3c::Right, FRotatec::CCW),    Face3Rotate(Face3c::Front, FRotatec::CCW),     Face3Rotate(Face3c::Back, FRotatec::CCW),
        Face3Rotate(Face3c::Back, FRotatec::CS),  Face3Rotate(Face3c::Front, FRotatec::CS),   Face3Rotate(Face3c::Left, FRotatec::CS),   Face3Rotate(Face3c::Right, FRotatec::CS),  Face3Rotate(Face3c::Bottom, FRotatec::CW),       Face3Rotate(Face3c::Top, FRotatec::CCW),
        Face3Rotate(Face3c::Top, FRotatec::CS),    Face3Rotate(Face3c::Bottom, FRotatec::N),        Face3Rotate(Face3c::Left, FRotatec::CCW),     Face3Rotate(Face3c::Right, FRotatec::CW),        Face3Rotate(Face3c::Back, FRotatec::CW),        Face3Rotate(Face3c::Front, FRotatec::CW),
        //右面在z+
        Face3Rotate(Face3c::Front, FRotatec::CW),         Face3Rotate(Face3c::Back, FRotatec::CCW),    Face3Rotate(Face3c::Top, FRotatec::CCW),      Face3Rotate(Face3c::Bottom, FRotatec::CCW),   Face3Rotate(Face3c::Right, FRotatec::N),         Face3Rotate(Face3c::Left, FRotatec::CS),
        Face3Rotate(Face3c::Bottom, FRotatec::CCW),   Face3Rotate(Face3c::Top, FRotatec::CCW),      Face3Rotate(Face3c::Front, FRotatec::CW),         Face3Rotate(Face3c::Back, FRotatec::CCW),    Face3Rotate(Face3c::Right, FRotatec::CCW),    Face3Rotate(Face3c::Left, FRotatec::CCW),
        Face3Rotate(Face3c::Back, FRotatec::CCW),    Face3Rotate(Face3c::Front, FRotatec::CW),         Face3Rotate(Face3c::Bottom, FRotatec::CCW),   Face3Rotate(Face3c::Top, FRotatec::CCW),      Face3Rotate(Face3c::Right, FRotatec::CS),  Face3Rotate(Face3c::Left, FRotatec::N),
        Face3Rotate(Face3c::Top, FRotatec::CCW),      Face3Rotate(Face3c::Bottom, FRotatec::CCW),   Face3Rotate(Face3c::Back, FRotatec::CCW),    Face3Rotate(Face3c::Front, FRotatec::CW),         Face3Rotate(Face3c::Right, FRotatec::CW),        Face3Rotate(Face3c::Left, FRotatec::CW),
        //右面在z-
        Face3Rotate(Face3c::Front, FRotatec::CCW),     Face3Rotate(Face3c::Back, FRotatec::CW),        Face3Rotate(Face3c::Bottom, FRotatec::CW),       Face3Rotate(Face3c::Top, FRotatec::CW),          Face3Rotate(Face3c::Left, FRotatec::CS),   Face3Rotate(Face3c::Right, FRotatec::N),
        Face3Rotate(Face3c::Bottom, FRotatec::CW),       Face3Rotate(Face3c::Top, FRotatec::CW),          Face3Rotate(Face3c::Back, FRotatec::CW),        Face3Rotate(Face3c::Front, FRotatec::CCW),     Face3Rotate(Face3c::Left, FRotatec::CCW),     Face3Rotate(Face3c::Right, FRotatec::CCW),
        Face3Rotate(Face3c::Back, FRotatec::CW),        Face3Rotate(Face3c::Front, FRotatec::CCW),     Face3Rotate(Face3c::Top, FRotatec::CW),          Face3Rotate(Face3c::Bottom, FRotatec::CW),       Face3Rotate(Face3c::Left, FRotatec::N),          Face3Rotate(Face3c::Right, FRotatec::CS),
        Face3Rotate(Face3c::Top, FRotatec::CW),          Face3Rotate(Face3c::Bottom, FRotatec::CW),       Face3Rotate(Face3c::Front, FRotatec::CCW),     Face3Rotate(Face3c::Back, FRotatec::CW),        Face3Rotate(Face3c::Left, FRotatec::CW),         Face3Rotate(Face3c::Right, FRotatec::CW),
    };
    Face3Rotate CubeFaceToward::face3ToStartFace3Rotate(Face3 face) const noexcept {
        return cubeFaceTowardTable[getRightToward() * 24 + getFRotate() * 6 + face];
    }

}