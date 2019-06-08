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

  auto view = aRegistry.view<ComponentPositionWorld, ComponentSprite, ComponentLayerBackground>();
  for(auto entity: view) 
  {
    auto& positionWorld = view.get<ComponentPositionWorld>( entity );
    auto& sprite = view.get<ComponentSprite>( entity );

    sf::Vector2f sfPosition(
      Globals::TILE_SIZE * positionWorld.mPosition.x,
      Globals::TILE_SIZE * positionWorld.mPosition.y );
    sprite.mSprite->setPosition( sfPosition );

    mRenderWindow->draw( *sprite.mSprite );
  }

  
  mRenderWindow->display();

  return true;
}

void
SystemRenderer::createMap( entt::registry& aRegistry, AssetLoader& aAssetsLoader )
{
  auto mapSize = aAssetsLoader.GetMapSize( AssetLoader::ASSET_MAP );
  auto map     = aAssetsLoader.GetMapData( AssetLoader::ASSET_MAP );
  std::shared_ptr<sf::Texture> texture = aAssetsLoader.GetTexture( AssetLoader::ASSET_TILEMAP );

  int entitiesToCreateCount = mapSize.x * mapSize.y;

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

}
