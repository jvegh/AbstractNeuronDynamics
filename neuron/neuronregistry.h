#pragma once

#include <QMetaEnum>
#include <QMetaType>
#include <QPointF>
#include <map>
#include <memory>

//#include "processors/interface/ripesprocessor.h"

namespace Neuron {
Q_NAMESPACE

template <typename T>
QString enumToString(T value) {
  int castValue = static_cast<int>(value);
  return QMetaEnum::fromType<T>().valueToKey(castValue);
}

// =============================== Neurons =================================
// The order of the NeuronID enum defines the order of which the neurons
// will appear in the neuron selection dialog.
enum NeuronID {
  Simple,
  NUM_NEURONS
};
Q_ENUM_NS(NeuronID); // Register with the metaobject system
// ============================================================================

#if 0
using RegisterInitialization =
    std::map<std::string_view, std::map<unsigned, VInt>>;
struct Layout {
  QString name;
  QString file;
  /**
   * @brief stageLabelPositions
   * Stage labels are not a part of the VSRTL processor model, and as such are
   * not serialized within the models layout. The first value in the points
   * determines the position of stage labels as a relative distance based on the
   * processor models' width in the VSRTL view. Should be in the range [0;1].
   * The second value in the point determines the y-position of the label, as a
   * multiple of the height of the font used. This is used so that multiple
   * labels can be "stacked" over one another. Must contain an entry for each
   * stage in the processor model.
   */
  std::map<StageIndex, QPointF> stageLabelPositions;
  bool operator==(const Layout &rhs) const { return this->name == rhs.name; }
};

#endif //0

class NeuronInfoBase {
public:
  NeuronInfoBase(NeuronID _id, const QString &_name, const QString &_desc
                   //,
//               const std::vector<Layout> &_layouts,
//               const RegisterInitialization &_defaultRegVals = {}
               )
      : id(_id), name(_name), description(_desc)
        //,
//        defaultRegisterVals(_defaultRegVals), layouts(_layouts)
        {}
  virtual ~NeuronInfoBase() = default;
  NeuronID id;
  QString name;
  QString description;
/*  RegisterInitialization defaultRegisterVals;
  std::vector<Layout> layouts;
  virtual ProcessorISAInfo isaInfo() const = 0;
  virtual std::unique_ptr<RipesProcessor>
  construct(const QStringList &extensions) = 0; */
};

template <typename T>
class NeuronInfo : public NeuronInfoBase {
public:
  using NeuronInfoBase::NeuronInfoBase;
  #if 0
  std::unique_ptr<RipesProcessor> construct(const QStringList &extensions) {
    return std::make_unique<T>(extensions); */
  }
  // At this point we force the processor type T to implement a static function
  // identifying its supported ISA.
//  ProcessorISAInfo isaInfo() const { return T::supportsISA(); }
#endif //0
};

class NeuronRegistry {
public:
  using NeuronMap = std::map<NeuronID, std::unique_ptr<NeuronInfoBase>>;
  static const NeuronMap &getAvailableNeurons() {
    return instance().m_descriptions;
  }
  #if 0
  static const ProcInfoBase &getDescription(ProcessorID id) {
    auto desc = instance().m_descriptions.find(id);
    if (desc == instance().m_descriptions.end()) {
      return *instance().m_descriptions.begin()->second;
    }
    return *desc->second;
  }
  static std::unique_ptr<RipesProcessor>
  constructProcessor(ProcessorID id, const QStringList &extensions) {
    auto &_this = instance();
    auto it = _this.m_descriptions.find(id);
    Q_ASSERT(it != _this.m_descriptions.end());
    return it->second->construct(extensions);
  }
#endif
private:
  template <typename T>
  void addNeuron(const NeuronInfo<T> &ninfo) {
    Q_ASSERT(m_descriptions.count(ninfo.id) == 0);
    m_descriptions[ninfo.id] = std::make_unique<NeuronInfo<T>>(ninfo);
  }

  NeuronRegistry();

  static NeuronRegistry &instance() {
    static NeuronRegistry n;
    return n;
  }

  NeuronMap m_descriptions;
}; // NeuronRegistry
} // namespace Neuron
