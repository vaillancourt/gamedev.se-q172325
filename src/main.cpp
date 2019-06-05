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
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "GlobalDefs.hpp"
#include "Components.hpp"
#include "Systems.hpp"
#include "AssetLoader.hpp"


void
createMap( entt::registry& aRegistry, AssetLoader& aAssetsLoader )
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


int main()
{

  std::shared_ptr<sf::RenderWindow> renderWindow = std::make_shared<sf::RenderWindow>( sf::VideoMode( 200, 200 ), "RPG test" );

  SystemRenderer systemRenderer( renderWindow );
  auto assetsLoader = std::make_unique<AssetLoader>();
  auto mapSize = assetsLoader->GetMapSize( AssetLoader::ASSET_MAP );

  entt::registry registry;

  createMap( registry, *assetsLoader );

  sf::CircleShape shape(100.f);
  shape.setFillColor(sf::Color::Green);
  while ( systemRenderer.render( shape ) )
  {
  }

  return 0;
}
