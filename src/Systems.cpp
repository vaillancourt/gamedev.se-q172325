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

#include "Systems.hpp"

#include <cassert>
#include <SFML/Graphics.hpp>

#include "AssetLoader.hpp"
#include "Components.hpp"
#include "GlobalDefs.hpp"

SystemRenderer::SystemRenderer( std::shared_ptr<sf::RenderWindow> aRenderWindow )
  : mRenderWindow ( aRenderWindow )
{
}


bool
SystemRenderer::render( entt::registry& aRegistry )
{
  if ( !mRenderWindow->isOpen() )
    return false;

  sf::Event event;
  while ( mRenderWindow->pollEvent( event ) )
  {
    if (event.type == sf::Event::Closed)
      mRenderWindow->close();
  }

  mRenderWindow->clear();

  auto viewBackground = aRegistry.view<ComponentPositionWorld, ComponentSprite, ComponentLayerBackground>();
  for(auto entity: viewBackground) 
  {
    auto& positionWorld = viewBackground.get<ComponentPositionWorld>( entity );
    auto& sprite = viewBackground.get<ComponentSprite>( entity );

    sf::Vector2f sfPosition(
      Globals::TILE_SIZE * positionWorld.mPosition.x,
      Globals::TILE_SIZE * positionWorld.mPosition.y );
    sprite.mSprite->setPosition( sfPosition );

    mRenderWindow->draw( *sprite.mSprite );
  }

  auto viewMainCharacterAnim = aRegistry.view<ComponentPositionWorld, ComponentSpriteAnimated, ComponentMainCharacter>();
  for(auto entity: viewMainCharacterAnim) 
  {
    auto& positionWorld = viewMainCharacterAnim.get<ComponentPositionWorld>( entity );
    auto& spriteAnimated = viewMainCharacterAnim.get<ComponentSpriteAnimated>( entity );

    sf::Vector2f sfPosition(
      Globals::TILE_SIZE * positionWorld.mPosition.x,
      Globals::TILE_SIZE * positionWorld.mPosition.y );
    spriteAnimated.mSequenceElements[spriteAnimated.mCurrentSequenceElementIndex]->mSprite->setPosition( sfPosition );

    mRenderWindow->draw( *spriteAnimated.mSequenceElements[spriteAnimated.mCurrentSequenceElementIndex]->mSprite );
  }
  
  mRenderWindow->display();

  return true;
}


void
SystemRenderer::updateAnimation( float aDt, entt::registry& aRegistry )
{
  auto view = aRegistry.view<ComponentSpriteAnimated>();
  for(auto entity: view) 
  {
    auto& spriteAnimated = view.get( entity );

    spriteAnimated.mTimeInCurrentLoop += aDt;
    if ( spriteAnimated.mTimeInCurrentLoop >= spriteAnimated.mTotalTime )
      spriteAnimated.mTimeInCurrentLoop -= spriteAnimated.mTotalTime;
    float currentRatio = spriteAnimated.mTimeInCurrentLoop / spriteAnimated.mTotalTime;

    float ratioAccumulator = 0.0f;
    for ( std::size_t i = 0u; i < spriteAnimated.mSequenceElements.size(); ++i )
    {
      ratioAccumulator += spriteAnimated.mSequenceElements[i]->mRatio;
      if ( currentRatio < ratioAccumulator )
        spriteAnimated.mCurrentSequenceElementIndex = i;
    }
  }
  
}


void
SystemRenderer::createMap( entt::registry& aRegistry, AssetLoader& aAssetsLoader )
{
  auto mapSize = aAssetsLoader.GetMapSize( AssetLoader::ASSET_MAP );
  auto map     = aAssetsLoader.GetMapData( AssetLoader::ASSET_MAP );
  std::shared_ptr<sf::Texture> texture = aAssetsLoader.GetTexture( AssetLoader::ASSET_TILEMAP );

 // int entitiesToCreateCount = mapSize.x * mapSize.y;

  for ( int y = 0; y < mapSize.y; ++y )
  {
    for ( int x = 0; x < mapSize.x; ++x )
    {
      sf::Vector2i spriteIndex = map[y][x];

      std::unique_ptr sprite = std::make_unique<sf::Sprite>();
      sprite->setTexture( *texture );
      sprite->setTextureRect( sf::IntRect( 
        spriteIndex.x * Globals::TILE_SIZE, 
        spriteIndex.y * Globals::TILE_SIZE, 
        Globals::TILE_SIZE, 
        Globals::TILE_SIZE ) );
      sprite->setOrigin( Globals::TILE_SIZE / 2, Globals::TILE_SIZE / 2 );

      auto entity = aRegistry.create();
      aRegistry.assign<ComponentPositionWorld>( entity, sf::Vector2f( static_cast<float>( x ), static_cast<float>( y ) ) );
      aRegistry.assign<ComponentSprite>( entity, texture, std::move( sprite ) );
      aRegistry.assign<ComponentLayerBackground>( entity );
    }
  }
}


void 
SystemRenderer::createMainAnimation( entt::registry& aRegistry, AssetLoader& aAssetsLoader )
{
  std::shared_ptr<sf::Texture> texture = aAssetsLoader.GetTexture( AssetLoader::ASSET_TILEMAP );
  std::vector<std::vector<AssetLoader::SequenceElement>> sequence = aAssetsLoader.GetMainAnimations( AssetLoader::ASSET_MAIN_ANIMATION );

  auto mainEntity = aRegistry.create();

  aRegistry.assign<ComponentPositionWorld>( mainEntity, sf::Vector2f( 20.0f, 20.0f ) );
  aRegistry.assign<ComponentMainCharacter>( mainEntity );
  
  ComponentSpriteAnimated& componentSpriteAnimated = aRegistry.assign<ComponentSpriteAnimated>( mainEntity );

  // We have 4 directions. 
  for ( int directionIndex = 0; directionIndex < ComponentCharacterAnimation::NUM_DIRECTIONS; ++directionIndex )
  {
    for ( std::size_t seqIndex = 0U; seqIndex < sequence[directionIndex].size(); ++seqIndex )
    {
      auto seqenceElement = std::make_unique<SequenceElement>();

      seqenceElement->mTexture = texture;
      seqenceElement->mSprite = std::make_unique<sf::Sprite>();
      seqenceElement->mSprite->setTexture( *seqenceElement->mTexture );
      seqenceElement->mSprite->setTextureRect( sf::IntRect( 
        sequence[directionIndex][seqIndex].mSpriteIndex.x * Globals::TILE_SIZE, 
        sequence[directionIndex][seqIndex].mSpriteIndex.y * Globals::TILE_SIZE, 
        Globals::TILE_SIZE, 
        Globals::TILE_SIZE ) );
      seqenceElement->mSprite->setOrigin( Globals::TILE_SIZE / 2, Globals::TILE_SIZE / 2 );

      componentSpriteAnimated.mSequenceElements.emplace_back( std::move( seqenceElement ) );
    }
  }


}
