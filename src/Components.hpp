/**
 * MIT License
 * 
 * Copyright (c) 2019 Alexandre Vaillancourt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <memory>
#include <array>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

struct ComponentPositionWorld
{
  sf::Vector2f mPosition;
};

struct ComponentSprite
{
  std::shared_ptr<sf::Texture> mTexture;
  std::unique_ptr<sf::Sprite> mSprite;
};

struct ComponentLayerBackground
{};

struct SequenceElement
{
  std::shared_ptr<sf::Texture> mTexture;
  std::unique_ptr<sf::Sprite> mSprite;
  float mRatio {1.0f};
  //~SequenceElement();
  //SequenceElement() = default;
  //SequenceElement( SequenceElement&& aOther )
  //{
  //  mTexture = std::move( aOther.mTexture );
  //  mSprite = std::move( aOther.mSprite );
  //  mRatio = aOther.mRatio;
  //}
};

struct ComponentSpriteAnimated
{
  float mTotalTime { 1.0f };
  float mTimeInCurrentLoop     { 0.0f };
  int mCurrentSequenceElementIndex { 0 };
  std::vector<std::unique_ptr<SequenceElement>> mSequenceElements;
};

struct ComponentWorldMovement
{
  sf::Vector2f mOrigin;
  sf::Vector2f mDestination;

  float mSpeed { 0.0f };
  float mRatio { 0.0f };
};

struct ComponentMainCharacter
{};

struct ComponentCharacterAnimation
{
  enum Direction
  {
    LEFT, DOWN, UP, RIGHT, NUM_DIRECTIONS
  };
  std::array<ComponentSpriteAnimated, NUM_DIRECTIONS> mMoveAnimations;
};
