#include "display.h"
#include "eadkpp.h"
#include "vector3.h"
#include "math.h"
struct rayResult
{
  unsigned char blockId = 0;
  char *face;
  unsigned char uv[2];
  float distance;
};
struct Player
{

  Vector3 position = {0.1, 3, 1.1};
  Vector3 rotation;
};

extern const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "numcraft";
extern const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;
const unsigned char pixelSize = 2;
const float PI = 3.1415;
const float fov = PI / 2;

rayResult ray(Vector3 *rayPos, Vector3 RayDir)
{
  Vector3 pos = {(*rayPos).x, (*rayPos).y, (*rayPos).z};
  short blockPos[3];
  Vector3 rayFrac = Vector3(pos.x - (double)((int)pos.x), pos.y - (double)((int)pos.y), pos.z - (double)((int)pos.z));
  unsigned char uv[2];
  rayResult rayInfo;
  char faces[3][3] = {
      {(RayDir.x > 0) ? -1 : 1, 0, 0},
      {0, (RayDir.y > 0) ? -1 : 1, 0},
      {0, 0, (RayDir.z > 0) ? -1 : 1}};
  float m;
  float t;
  short iteration = 0;
  char faceNormal[3] = {(RayDir.x > 0) ? -1 : 1, 0, 0};
  while (((pos.sub((*rayPos))).squareLength() < 20) && iteration < 20)
  {
    m = ((1 - ((faces[0][0] + 1) * 0.5) - rayFrac.x)) / (RayDir.x);
    t = ((1 - ((faces[1][1] + 1) * 0.5) - rayFrac.y)) / (RayDir.y);
    if (t > 0 && t < m)
    {
      m = t;
      faceNormal[0] = 0;
      faceNormal[1] = faces[1][1];
    }
    t = ((1 - ((faces[2][2] + 1) * 0.5) - rayFrac.z)) / (RayDir.z);
    if (t > 0 && t < m)
    {
      m = t;
      faceNormal[0] = 0;
      faceNormal[1] = 0;
      faceNormal[2] = faces[2][2];
    }
    pos = pos.add(RayDir.mul(m + 0.0001));
    // rayFrac=Vector3(pos.x-(double)((int)pos.x),pos.y-(double)((int)pos.y),pos.z-(double)((int)pos.z));
    blockPos[0] = (pos.x < 0) ? ((short)pos.x) - 1 : (short)pos.x;
    blockPos[1] = (short)pos.y;
    blockPos[2] = (pos.z < 0) ? ((short)pos.z) - 1 : (short)pos.z;
    if (blockPos[1] < 2)
    {
      rayInfo.blockId = 1;
      rayInfo.distance = (float)(pos.sub((*rayPos))).squareLength();
      rayInfo.uv[0] = (faceNormal[0] != 0) ? (int)(rayFrac.z * 8) : (int)(rayFrac.x * 8);
      rayInfo.uv[1] = (faceNormal[1] != 0) ? (int)(rayFrac.z * 8) : (int)(rayFrac.y * 8);
    }
  }
  return rayInfo;
}

void renderFrame(Player *_player)
{
  float xangleReset = -fov / 2;
  float xangleStep = fov / (320 / pixelSize);
  float yangleStep = fov / (320 / pixelSize);
  float xangle = xangleReset;
  float yangle = -xangle * 0.75;
  Vector3 skyColor[2] = {{40, 40, 100}, {66, 135, 245}};
  Vector3 rayDir;
  const double rCos[2] = {cos((*_player).rotation.x), cos((*_player).rotation.y)};
  const double rSin[2] = {sin((*_player).rotation.x), sin((*_player).rotation.y)};

  // def rotateVec(rayDir:V,rcos:V,rsin:V)->V:
  // return V((rcos.val[1]*rayDir.val[0]+rayDir.val[1]*(rsin.val[1]*rsin.val[0])+rayDir.val[2]*(rsin.val[1]*rcos.val[0]),rayDir.val[1]*(rcos.val[0])+rayDir.val[2]*(-rsin.val[0]),-rsin.val[1]*rayDir.val[0]+rayDir.val[1]*(rcos.val[1]*rsin.val[0])+rayDir.val[2]*(rcos.val[1]*rcos.val[0])))

  for (int y = 0; y < 240; y += pixelSize)
  {
    for (int x = 0; x < 320; x += pixelSize)
    {
      if (((160 - x) * (160 - x)) + ((120 - y) * (120 - y)) < 16)
      {
        EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(0xFFFFFF));
      }
      else
      {
        rayDir = {rCos[1] * xangle + yangle * (rSin[1] * rSin[0]) + (rSin[1] * rCos[0]), yangle * (rCos[0]) - rSin[0], -rSin[1] * xangle + yangle * (rCos[1] * rSin[0]) + (rCos[1] * rCos[0])};
        rayDir = rayDir.normalize();
        // rayResult rayInfo=ray(&((*_player).position),rayDir);
        /*rayResult rayInfo=ray(&((*_player).position),rayDir);
        if (rayInfo.blockId)
        {

          EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color((rayInfo.uv[0]>0 && rayInfo.uv[1]>0 && rayInfo.uv[0]<7 && rayInfo.uv[1]<7)?0xFF0000:0x00FF00));
        }
        else{*/

        double sunDotProduct = rayDir.dot({0, 0.45, 0.9});
        if (sunDotProduct > 0.999)
        {
          EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(0xf8f8e6));
        }
        else if (sunDotProduct > 0.99)
        {
          // elif rayDir.dot(V((0,0.45,0.9)))>0.99: a=1-(111*(0.999-rayDir.dot(V((0,0.45,0.9)))));skyColor=((V((66, 135, 245))*V((1-a,1-a,1-a)))+ (V((238,238,200))*V((a,a,a))))
          double sunBorderScalar = 1 - (111 * (0.999 - sunDotProduct));
          EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color((skyColor[1].mul(1 - sunBorderScalar).add(Vector3(238, 238, 200).mul(sunBorderScalar))).toColor()));
        }
        else
        {

          if (rayDir.y > 0.1)
          {
            EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(skyColor[1].toColor()));
          }
          else if (rayDir.y < -0.1)
          {
            EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(skyColor[0].toColor()));
          }
          else
          {
            double c = (double)((rayDir.y + 0.1) * 5);
            EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color((skyColor[0].mul(1 - c).add(skyColor[1].mul(c))).toColor()));
          }
        }
      }

      xangle += xangleStep;
    }
    xangle = xangleReset;
    yangle -= yangleStep;
  }
}

int main(int argc, char *argv[])
{

  bool discretMode = false;
  Player player;
  player.rotation = {-0.1, 0, 0};
  renderFrame(&player);

  while (1)
  {

    EADK::Keyboard::State keyboardState = EADK::Keyboard::scan();
    if (!discretMode)
    {
      if (keyboardState.keyDown(EADK::Keyboard::Key::XNT))
      {
        renderFrame(&player);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Right))
      {
        player.rotation.y -= 0.2;
        if (player.rotation.y < 0)
        {
          player.rotation.y += 6.28318530718;
        }
        renderFrame(&player);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Left))
      {
        player.rotation.y += 0.2;
        if (player.rotation.y > 6.28318530718)
        {
          player.rotation.y -= 6.28318530718;
        }
        renderFrame(&player);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Up))
      {
        player.rotation.x -= 0.2;
        if (player.rotation.y < -1.57079632679)
        {
          player.rotation.y = -1.57079632679;
        }
        renderFrame(&player);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Down))
      {
        player.rotation.x += 0.2;
        if (player.rotation.y > 1.57079632679)
        {
          player.rotation.y = 1.57079632679;
        }
        renderFrame(&player);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::XNT))
      {
        renderFrame(&player);
      }
    }

    if (keyboardState.keyDown(EADK::Keyboard::Key::Minus))
    {
      break;
    }
    if (keyboardState.keyDown(EADK::Keyboard::Key::Plus))
    {
      discretMode = !discretMode;
      if (!discretMode)
      {
        renderFrame(&player);
      }
      else
      {
        EADK::Display::pushRectUniform(EADK::Rect(0, 0, 320, 18), EADK::Color(0xFFB531));
        EADK::Display::pushRectUniform(EADK::Rect(0, 18, 320, 192), EADK::Color(0xF6FAFF));
        EADK::Display::pushRectUniform(EADK::Rect(0, 205, 320, 35), EADK::Color(0xfffeff));
        EADK::Display::pushRectUniform(EADK::Rect(0, 205, 320, 2), EADK::Color(0xeceaec));

        // EADK::Display::drawString("rad",EADK::Point(5,3),false,EADK::Color(0xFFFFFF), EADK::Color(0xFFB531));
        // EADK::Display::drawString("CALCULS",EADK::Point(137,4),false,EADK::Color(0xFFFFFF), EADK::Color(0xFFB531));
      }

      EADK::Timing::msleep(200);
    }
  }
}
