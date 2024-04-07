#include "display.h"
#include "eadkpp.h"
#include "vector3.h"
#include "math.h"
#include "texture.h"
#include "world.h"
#include "ui.h"
struct rayResult
{
  unsigned char blockId = 0;
  unsigned char face;
  signed char blockPos[3];
  double distance;
  unsigned short color;
  unsigned char skyFactor = 0;
};
struct Player
{
  Vector3 position = {1.1, 3, 1.3};
  Vector3 rotation = {0.1, 0.1, 0};
};

extern const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "numcraft";
extern const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;
const unsigned char pixelSize = 4;
const float PI = 3.1415;
const float fov = PI / 2;
bool isInventoryOpen = false;
unsigned char selectedSlotPos[2] = {1, 1};
rayResult cursorRayResult;
rayResult rayInfo;
rayResult ray(Vector3 *rayPos, Vector3 *RayDir)
{
  Vector3 pos = {rayPos->x, rayPos->y, rayPos->z};
  Vector3 dir = {RayDir->x, RayDir->y, RayDir->z};
  signed char blockPos[3];
  Vector3 rayFrac = Vector3((pos.x > 0) ? pos.x - (double)((int)pos.x) : 1.0 + (pos.x - (double)((int)pos.x)), (pos.y > 0) ? pos.y - (double)((int)pos.y) : 1.0 + (pos.y - (double)((int)pos.y)), (pos.z > 0) ? pos.z - (double)((int)pos.z) : 1.0 + (pos.z - (double)((int)pos.z)));
  unsigned char uv[2];
  rayResult rayInfo;
  // return rayInfo;
  const signed char faces[3][3] = {
      {(dir.x > 0.0) ? -1 : 1, 0, 0},
      {0, (dir.y > 0.0) ? -1 : 1, 0},
      {0, 0, (dir.z > 0.0) ? -1 : 1}};
  double m;
  double t;
  unsigned char iteration = 0;
  double distance = 0;
  unsigned char faceNormal;
  const double inverses[3] = {1 / dir.x, 1 / dir.y, 1 / dir.z};
  do
  {
    m = ((faces[0][0] == -1) - rayFrac.x) * inverses[0];
    faceNormal = 0;
    t = ((faces[1][1] == -1) - rayFrac.y) * inverses[1];
    if (t < m)
    {
      m = t;
      faceNormal = 1;
    }
    t = ((faces[2][2] == -1) - rayFrac.z) * inverses[2];
    if (t < m)
    {
      m = t;
      faceNormal = 2;
    }
    pos = pos.add((dir).mul(m + 0.001));
    if (pos.y > 16 || pos.y < 0)
    {
      rayInfo.blockId = 0;
      return rayInfo;
    }
    distance += m + 0.0001;
    if (distance > 8)
    {
      rayInfo.distance = distance;
      return rayInfo;
    }
    rayFrac = Vector3((pos.x > 0) ? pos.x - (double)((int)pos.x) : 1.0 + (pos.x - (double)((int)pos.x)), (pos.y > 0) ? pos.y - (double)((int)pos.y) : 1.0 + (pos.y - (double)((int)pos.y)), (pos.z > 0) ? pos.z - (double)((int)pos.z) : 1.0 + (pos.z - (double)((int)pos.z)));
    blockPos[0] = (pos.x < 0) ? ((signed char)pos.x) - 1 : (signed char)pos.x;
    blockPos[1] = (signed char)pos.y;
    blockPos[2] = (pos.z < 0) ? ((signed char)pos.z) - 1 : (signed char)pos.z;
    unsigned char blockId = getBlockFromWorld(&blockPos[0]);
    if (blockPos[1] > 16 || blockPos[1] < 0)
    {
      rayInfo.distance = distance;
      return rayInfo;
    }
    if (blockId != 0)
    {
      rayInfo.blockId = blockId;
      rayInfo.distance = distance;
      unsigned char face = 0;
      uv[0] = (unsigned char)(rayFrac.z * 8);
      uv[1] = (unsigned char)(rayFrac.y * 8);
      if (faces[faceNormal][0] == -1)
      {
        face = 1;
      }
      else if (faces[faceNormal][1] == 1)
      {
        uv[0] = (unsigned char)(rayFrac.x * 8);
        uv[1] = (unsigned char)(rayFrac.z * 8);
        face = 2;
      }
      else if (faces[faceNormal][1] == -1)
      {
        uv[0] = (unsigned char)(rayFrac.x * 8);
        uv[1] = (unsigned char)(rayFrac.z * 8);
        face = 3;
      }
      else if (faces[faceNormal][2] == 1)
      {
        uv[0] = (unsigned char)(rayFrac.x * 8);
        uv[1] = (unsigned char)(rayFrac.y * 8);
        face = 4;
      }
      else if (faces[faceNormal][2] == -1)
      {
        uv[0] = (unsigned char)(rayFrac.x * 8);
        uv[1] = (unsigned char)(rayFrac.y * 8);
        face = 5;
      }
      rayInfo.blockPos[0] = (signed char)blockPos[0];
      rayInfo.blockPos[1] = (signed char)blockPos[1];
      rayInfo.blockPos[2] = (signed char)blockPos[2];
      rayInfo.color = getPixelFromTexture(blockId, uv[0], uv[1], face);
      rayInfo.face = face;
      return rayInfo;
    }
    iteration++;
  } while (distance < 8 && iteration < 13);
  return rayInfo;
}

void renderFrame(Player *_player, double dayTime, bool stopWithKey)
{
  // render a frame where stopWith determine if a key is pressed if it should stop the render

  double xangleReset = -fov / 2;
  double xangleStep = fov / (320 / pixelSize);
  double yangleStep = fov / (320 / pixelSize);
  double xangle = xangleReset;
  double yangle = -xangle * 0.75;
  double eveningScalar;
  double morningScalar;
  bool isMorning = false;
  bool isEvening = false;
  const Vector3 skyColor[4] = {{40, 40, 100}, {66, 135, 245}, {0, 7, 41}};
  const Vector3 sunColor[3] = {{255, 247, 186}, {248, 248, 230}, {255, 178, 77}};
  const Vector3 sunOutlineColor[3] = {{250, 255, 150}, {238, 238, 200}, {255, 159, 48}};
  Vector3 rayDir;
  Vector3 upperSkyColor, lowerSkyColor;
  Vector3 sunColor_, sunOutlineColor_;
  const double rCos[2] = {cos((*_player).rotation.x), cos((*_player).rotation.y)};
  const double rSin[2] = {sin((*_player).rotation.x), sin((*_player).rotation.y)};
  const EADK::Keyboard::Key stopKeys[] = {EADK::Keyboard::Key::XNT,
                                          EADK::Keyboard::Key::Down,
                                          EADK::Keyboard::Key::Up,
                                          EADK::Keyboard::Key::Left,
                                          EADK::Keyboard::Key::Right,
                                          EADK::Keyboard::Key::Alpha,
                                          EADK::Keyboard::Key::Ln,
                                          EADK::Keyboard::Key::Backspace,
                                          EADK::Keyboard::Key::Power,
                                          EADK::Keyboard::Key::Toolbox,
                                          EADK::Keyboard::Key::Shift,
                                          EADK::Keyboard::Key::Comma,
                                          EADK::Keyboard::Key::Var};
  Vector3 cursorDir = {rCos[1] * (xangleReset + (xangleStep * 160 / pixelSize)) + (yangle - (yangleStep * 120 / pixelSize)) * (rSin[1] * rSin[0]) + (rSin[1] * rCos[0]), (yangle - (yangleStep * 120 / pixelSize)) * (rCos[0]) - rSin[0], -rSin[1] * (xangleReset + (xangleStep * 160 / pixelSize)) + (yangle - (yangleStep * 120 / pixelSize)) * (rCos[1] * rSin[0]) + (rCos[1] * rCos[0])};
  rayResult cursorRayInfo = (ray(&((*_player).position), &cursorDir));
  if (cursorRayInfo.blockId == 0 || cursorRayInfo.distance > 4)
  {
    cursorRayInfo.blockPos[1] = -2;
  }
  cursorRayResult = cursorRayInfo;
  if (dayTime > 2.47492598692 && dayTime < 3.14159265359)
  {
    eveningScalar = (1.5 * (dayTime - 3.14159265359));
    eveningScalar *= eveningScalar;
    upperSkyColor = gradient(skyColor[2], skyColor[1], eveningScalar);
    sunColor_ = gradient(sunColor[2], sunColor[1], eveningScalar);
    sunOutlineColor_ = gradient(sunOutlineColor[2], sunOutlineColor[1], eveningScalar);
    lowerSkyColor = skyColor[0].mul(eveningScalar);
    isEvening = true;
  }
  else if (dayTime > 5.95)
  {
    morningScalar = (3 * (dayTime - 6.28318530718));
    morningScalar *= morningScalar;
    upperSkyColor = gradient(skyColor[1], skyColor[2], morningScalar);
    lowerSkyColor = skyColor[1].mul(1 - morningScalar);
    sunColor_ = gradient(sunColor[1], sunColor[0], morningScalar);
    sunOutlineColor_ = gradient(sunOutlineColor[1], sunOutlineColor[0], morningScalar);
    isMorning = true;
  }
  else if (dayTime < 2.47492598692)
  {
    sunColor_ = sunColor[1];
    sunOutlineColor_ = sunOutlineColor[1];
    upperSkyColor = skyColor[1];
    lowerSkyColor = skyColor[0];
    //
  }
  else
  {
    upperSkyColor = skyColor[2];
    lowerSkyColor = Vector3(0, 0, 0);
    sunColor_ = sunColor[2];
    sunOutlineColor_ = sunOutlineColor[2];
  }

  for (unsigned char y = 0; y < 240; y += pixelSize)
  {

    if (stopWithKey)
    {
      for (int k = 0; k < 13; k++)
      {
        if (EADK::Keyboard::scan().keyDown(stopKeys[k]))
        {
          return;
        }
      }
    }
    for (unsigned short x = 0; x < 320; x += pixelSize)
    {

      if (((160 - x) * (160 - x)) + ((120 - y) * (120 - y)) < 16)
      {
        EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(0xFFFFFF));
      }
      else
      {

        rayDir = {rCos[1] * xangle + yangle * (rSin[1] * rSin[0]) + (rSin[1] * rCos[0]), yangle * (rCos[0]) - rSin[0], -rSin[1] * xangle + yangle * (rCos[1] * rSin[0]) + (rCos[1] * rCos[0])};
        rayDir = rayDir.normalize();

        rayResult rayInfo = ray((&(*_player).position), &rayDir);
        if (rayInfo.blockId != 0 && rayInfo.distance < 7.5)
        {
          Vector3 color = {(double)((rayInfo.color >> 11) * 8), (double)(((rayInfo.color >> 5) & 0b0000000000111111) * 4), (double)((rayInfo.color & 0b0000000000011111) * 8)};
          if (rayInfo.blockPos[0] == cursorRayInfo.blockPos[0] && rayInfo.blockPos[1] == cursorRayInfo.blockPos[1] && rayInfo.blockPos[2] == cursorRayInfo.blockPos[2])
          {
            color = color.add(Vector3(255, 255, 255)).mul(0.5);
          }
          if (dayTime > 3.1415)
          {
            color = color.mul(0.8);
          }
          if (isMorning)
          {
            color = color.mul(0.8 + (1 - morningScalar) * 0.2);
          }
          if (isEvening)
          {
            color = color.mul(0.8 + eveningScalar * 0.2);
          }
          EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(color.toColor()));
        }
        else
        {
          Vector3 backgroundSkyColor;
          if (rayDir.y > 0)
          {
            backgroundSkyColor = upperSkyColor;
          }
          else if (rayDir.y < -0.2)
          {
            backgroundSkyColor = lowerSkyColor;
          }
          else
          {
            double c = (double)((rayDir.y + 0.2) * 5);
            backgroundSkyColor = gradient(lowerSkyColor, upperSkyColor, c);
          }
          double sunDotProduct = rayDir.dot({0, sin(dayTime), -cos(dayTime)});
          if (sunDotProduct > 0.85)
          {
            if (dayTime > 2.8 && dayTime < 5)
            {
              backgroundSkyColor = gradient(gradient(Vector3(242, 83, 44), backgroundSkyColor, (0.99 - sunDotProduct) * 7.142857), backgroundSkyColor, (1 - (dayTime - 2.8) > 0.5) ? 1 - (dayTime - 2.8) : 0.5);
            }
          }
          if (sunDotProduct > 0.99)
          {
            backgroundSkyColor = sunColor_;
          }
          else if (sunDotProduct > 0.985)
          {
            double sunBorderScalar = 1 - (200 * (0.99 - sunDotProduct));

            backgroundSkyColor = gradient(backgroundSkyColor, sunOutlineColor_, sunBorderScalar);
          }
          double moonDotProduct = rayDir.dot({0, -sin(dayTime), cos(dayTime)});
          if (moonDotProduct > 0.99 && rayDir.dot({0.12, -sin(dayTime), cos(dayTime)}) < 1.005)
          {
            backgroundSkyColor = Vector3(248, 248, 230);
          }
          if (rayInfo.blockId != 0 && rayInfo.distance < 8)
          {
            Vector3 color = {(double)((rayInfo.color >> 11) * 8), (double)(((rayInfo.color >> 6) & 0b0000000000011111) * 8), (double)((rayInfo.color & 0b0000000000011111) * 4)};
            if (dayTime > 3.1415)
            {
              color = color.mul(0.8);
            }
            if (isMorning)
            {
              color = color.mul(0.8 + (1 - morningScalar) * 0.2);
            }
            if (isEvening)
            {
              color = color.mul(0.8 + eveningScalar * 0.2);
            }
            backgroundSkyColor = gradient(backgroundSkyColor, color, (8 - rayInfo.distance) * 2);
          }
          EADK::Display::pushRectUniform(EADK::Rect(x, y, pixelSize, pixelSize), EADK::Color(backgroundSkyColor.toColor()));
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
  initWorld();
  double dayTime = 2;
  bool discretMode = false;
  Player player;
  player.rotation = {0.1, 0.1, 0};
  player.position = {1.1, 4.2, 1.03};
  renderFrame(&player, 2, false);

  while (1)
  {
    if (dayTime > 6.28318530718)
    {
      dayTime = 0;
    }
    EADK::Keyboard::State keyboardState = EADK::Keyboard::scan();
    if (!discretMode)
    {

      if (!isInventoryOpen)
      {
        dayTime += 0.01;
        renderFrame(&player, dayTime, true);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::XNT))
      {
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Left))
      {
        player.rotation.y -= 0.2;
        if (player.rotation.y < 0)
        {
          player.rotation.y = 6.27318530718;
        }
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Right))
      {
        player.rotation.y += 0.2;
        if (player.rotation.y > 6.28318530718)
        {
          player.rotation.y = 0.01;
        }
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Up))
      {
        player.rotation.x -= 0.2;
        if (player.rotation.x < -1.57079632679)
        {
          player.rotation.x = -1.57079632679;
        }
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Alpha))
      {
        if (cursorRayResult.blockPos[1] >= 0)
        {
          signed char blockPos[3] = {cursorRayResult.blockPos[0], cursorRayResult.blockPos[1], cursorRayResult.blockPos[2]};

          if (cursorRayResult.face == 1)
          {
            blockPos[0] += -1;
          }
          else if (cursorRayResult.face == 0)
          {
            blockPos[0] += 1;
          }
          if (cursorRayResult.face == 3)
          {
            blockPos[1] += -1;
          }
          else if (cursorRayResult.face == 2)
          {
            blockPos[1] += 1;
          }
          if (cursorRayResult.face == 5)
          {
            blockPos[2] += -1;
          }
          else if (cursorRayResult.face == 4)
          {
            blockPos[2] += 1;
          }

          changeBlockInWorld(&(blockPos[0]), 3);
        }
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Ln))
      {

        if (cursorRayResult.blockPos[1] >= 0)
        {
          changeBlockInWorld(&(cursorRayResult.blockPos[0]), 0);
        }
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Down))
      {
        player.rotation.x += 0.2;
        if (player.rotation.x > 1.57079632679)
        {
          player.rotation.x = 1.57079632679;
        }
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Backspace))
      {
        player.position.z += cos(player.rotation.y);
        player.position.x += sin(player.rotation.y);
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Power))
      {
        player.position.z -= cos(player.rotation.y);
        player.position.x -= sin(player.rotation.y);
        dayTime += 0.1;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Toolbox))
      {
        player.position.y += 1;
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (!isInventoryOpen && keyboardState.keyDown(EADK::Keyboard::Key::Comma))
      {
        player.position.y -= 1;
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Var))
      {
        selectedSlotPos[0] = 1;
        selectedSlotPos[1] = 2;
        isInventoryOpen = !isInventoryOpen;
        if (isInventoryOpen)
        {
          draw_inventory(&(selectedSlotPos[0]));
        }
        EADK::Timing::msleep(200);
        // renderFrame(&player, dayTime, false);
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Right) && isInventoryOpen)
      {
        if (selectedSlotPos[0] < 4)
        {
          draw_selectedInventorySlot(false, &(selectedSlotPos[0]));
          selectedSlotPos[0]++;
          draw_selectedInventorySlot(true, &(selectedSlotPos[0]));
          EADK::Timing::msleep(200);
        }
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Left) && isInventoryOpen)
      {
        if (selectedSlotPos[0] > 0)
        {
          draw_selectedInventorySlot(false, &(selectedSlotPos[0]));
          selectedSlotPos[0]--;
          draw_selectedInventorySlot(true, &(selectedSlotPos[0]));
          EADK::Timing::msleep(200);
        }
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Up) && isInventoryOpen)
      {
        if (selectedSlotPos[1] > 0)
        {
          draw_selectedInventorySlot(false, &(selectedSlotPos[0]));
          selectedSlotPos[1]--;
          draw_selectedInventorySlot(true, &(selectedSlotPos[0]));
          EADK::Timing::msleep(200);
        }
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Down) && isInventoryOpen)
      {
        if (selectedSlotPos[1] < 2)
        {
          draw_selectedInventorySlot(false, &(selectedSlotPos[0]));
          selectedSlotPos[1]++;
          draw_selectedInventorySlot(true, &(selectedSlotPos[0]));
          EADK::Timing::msleep(200);
        }
      }
      if (keyboardState.keyDown(EADK::Keyboard::Key::Shift))
      {

        if (player.rotation.y + 3.14159265359 > 6.28318530718)
        {
          player.rotation.y -= 3.14159265359;
        }
        else
        {
          player.rotation.y += 3.14159265359;
        }
        dayTime += 0.03;
        renderFrame(&player, dayTime, false);
      }
    }
    if (keyboardState.keyDown(EADK::Keyboard::Key::XNT))
    {
      dayTime += 0.5;
      renderFrame(&player, dayTime, false);
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
        dayTime += 0.1;
        renderFrame(&player, dayTime, false);
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

      EADK::Timing::msleep(400);
    }
  }
}
