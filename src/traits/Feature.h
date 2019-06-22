/*
 * Feature.h
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_FEATURE_H_
#define SRC_TRAITS_FEATURE_H_

#include <string>
#include <map>

#include "Enum.h"

class Feature: public Annotation
{
public:
  /**
   * Default constructor
   */
  Feature();

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  Feature(const Feature & src);

  /**
   * Destructor
   */
  virtual ~Feature();

  void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  size_t size() const;

  size_t bitsRequired() const;

  void setMask(const TraitData & mask);

  const TraitData & getMask() const;

  const Enum & getEnum(const std::string & ) const;

private:
  std::string mId;
  std::string mDefault;
  TraitData mMask;
  std::map< std::string, Enum > mEnumMap;
  bool mValid;
};

#endif /* SRC_TRAITS_FEATURE_H_ */
