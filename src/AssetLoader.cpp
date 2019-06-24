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
#include "AssetLoader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include "GlobalDefs.hpp"

std::shared_ptr<sf::Texture> 
AssetLoader::GetTexture( Asset aAsset )
{
  if ( auto asset = mTextures.find( aAsset );
    asset != mTextures.end() )
    return asset->second;

  auto newTexture = std::make_shared<sf::Texture>();

  std::string path;

  switch ( aAsset )
  {
  case ASSET_TILEMAP: path = Globals::TILE_MAP;
  }

  if ( !newTexture->loadFromFile( path ) )
  {
    assert( false );
  }

  mTextures[aAsset] = newTexture;

  return newTexture;
}

sf::Vector2i 
AssetLoader::GetMapSize( Asset aAsset )
{
  if ( mMaps.count( aAsset ) == 0 )
    loadMap( aAsset );

  const auto& map = mMaps.find( aAsset )->second;

  return { static_cast<int>( map[0].size() ), static_cast<int>( map.size() ) };
}


std::vector<std::vector<sf::Vector2i>> 
AssetLoader::GetMapData( Asset aAsset )
{
  if ( mMaps.count( aAsset ) == 0 )
    loadMap( aAsset );

  const auto& map = mMaps.find( aAsset )->second;

  return map;
}


void 
AssetLoader::loadMap( Asset aAsset )
{
  if ( mMaps.count( aAsset ) > 0 )
    return;

  std::string path;

  switch ( aAsset )
  {
  case ASSET_MAP: path = Globals::MAP;
  }

  std::ifstream reader( path );

  if ( !reader.is_open() )
  {
    assert( false );
  }

  int width;
  int height;

  reader >> width >> height;

  std::vector<std::vector<sf::Vector2i>> map;
  map.resize( height );

  for ( int y = 0; y < height; ++y )
  {
    map[y].resize( width );

    for ( int x = 0; x < width; ++x )
    {
      reader >> map[y][x].x >> map[y][x].y;
    }
  }
  
  reader.close();

  mMaps[aAsset] = map;
}

std::vector<std::vector<AssetLoader::SequenceElement>> 
AssetLoader::GetMainAnimations( Asset aAsset )
{
  std::vector<std::vector<SequenceElement>> retVal;

  std::string path;

  switch ( aAsset )
  {
  case ASSET_MAIN_ANIMATION: path = Globals::ANIMATION;
  }

  std::ifstream reader( path );

  if ( !reader.is_open() )
  {
    assert( false );
  }

  // We have 4 directions. 
  for ( int i = 0; i < ComponentCharacterAnimation::NUM_DIRECTIONS; ++i )
  {
    std::vector<SequenceElement> thisSequence;
    int elementsInSequence;
    reader >> elementsInSequence;
    for ( int sequenceIndex = 0; sequenceIndex < elementsInSequence; ++sequenceIndex )
    {
      SequenceElement sequenceElement;

      reader >> sequenceElement.mSpriteIndex.x >> sequenceElement.mSpriteIndex.y >> sequenceElement.mRatio;

      thisSequence.push_back( sequenceElement );
    }
    retVal.push_back( thisSequence );
  }

  reader.close();

  return retVal;
}
