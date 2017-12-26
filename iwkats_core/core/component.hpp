#pragma once

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <ostream>
#include <iostream>
#include "util.hpp"

#include "iwkats_core_global.h"

using std::unique_ptr;
using std::shared_ptr;
using std::string;

/**
 * Exception due to violation of logic rules of Component.
 * The error is fatal and should be immediately corrected.
 */
class IWKATS_CORE_EXPORT TopologyException : public std::logic_error {
public:
  TopologyException(const string &msg) : logic_error(msg) { }
};

/**
 * Thrown by Component instances interacting with files, e.g., Reader, Writer
 * as well as configuration parsing classes.
 * The exception could lead to dysfunction of some classes.
 */
class IWKATS_CORE_EXPORT DataIOException : public std::ios_base::failure {
public:
  DataIOException(const string &msg) : std::ios_base::failure(msg) { }
};

/** Base class for topology component.
 * A topology component is distinguished by its id, which is set
 * at instantiation. The base class also provides logging utilities.
 */
class IWKATS_CORE_EXPORT Component {
protected:
  static Logger logger; //!< logger shared by all Component instances
  const string id;
public:
  Component(const string &componentId) : id(componentId) { }

  string getComponentId() const { return id; }

  /** Logging method.
     * Always use this method to print/log information! Change output to redirect message
     * @param level Available options: Logger::INFO, Logger::DBG, Logger::ERR
     */
  void log(const string &msg, Logger::LogLevel level = Logger::INFO, std::ostream &output = std::cout) {
    //logger.log(id, msg, level, output);
  }

  /** Logging method.
     * The method provides inline conveniently outputs success or failure message depending on a flag.
     * Log level will be set to Logger::ERR when there is an error, or to Logger::INFO otherwise.
     * @param error Indicates whether the operation was successful (0 for no error)
     */
  void log(const string &successMsg, const string &failureMsg, int error, std::ostream &output = std::cout) {
    if (error == 0)
      logger.log(id, successMsg, Logger::INFO, output);
    else
      logger.log(id, failureMsg, Logger::ERR, output);
  }
};


/** A data stream processing Component in topology.
 * Processor receives data stream via Processor::process.
 * Subclasses inheriting Processor must implement Processor::process.
 */
template<typename T>
class IWKATS_CORE_EXPORT Processor : public Component {
public:
  Processor(const string &componentId) : Component(componentId) { };

  virtual ~Processor() { };

  /** Logic to process incoming data should be implemented here.
     * Copy of the shared pointer is encouraged when storing data (no additional space will be consumed).
     */
  virtual void process(const shared_ptr<T> &) = 0;
};


/** A data stream generating Component in topology.
 * DataHub uses a broadcast model which send data to all interested Processor instances via DataHub::dispatch.
 */
template<typename T>
class IWKATS_CORE_EXPORT DataHub : public Component {
protected:
  const static string DEFAULT_STREAM_ID; // defined below, valued "_DEFAULT_"

  EasyMap<string, std::vector<shared_ptr<Processor<T>>>> subscribers; //!< streamID => interested Processor instances

public:
  /**
     * A default stream id is always declared upon instantiation.
     */
  DataHub(string datahubId) : Component(datahubId) {
    declareStream(DEFAULT_STREAM_ID);
  };

  virtual ~DataHub() { };

  /** Declares what channels of stream this DataHub will be producing.
     * For example, a CTPMarket might provide market data for several market instruments.
     * The names of data streams should be declared here in the first place so that
     * downstream Processor instances can be attached to them.
     * This method allows no duplicate stream names.
     */
  void declareStream(const string &streamId) {
    // ensure no key duplicates
    if (!subscribers.contains(streamId)) {
      subscribers[streamId] = std::vector<shared_ptr<Processor<T>>>();
    }
  }

  /**
     * Call to start the driving DataHub (i.e., the very first DataHub in topology)
     */
  virtual void run() { };

  /** To register a Processor as the receiver of a specific stream produced by this DataHub.
     * The method throws KeyErrorException if the target streamId has not been previously declared.
     * If there is no streamId specified, Processor subscribes to DEFAULT_STREAM_ID.
     */
  virtual void attach(const shared_ptr<Processor<T>> &processor, const string &streamId = DEFAULT_STREAM_ID) {
    try {
      subscribers.get(streamId).push_back(processor);
    } catch (KeyErrorException &e) {
      throw TopologyException("Attaching error: Datahub[" + streamId + "] has not declared stream " + streamId);
    }
  }

  /**
     * To forward data to all interested Processor instances.
     * called by child #DataHub#.
     * If no stream id specified, data will be forwarded to all #Processor#s subscribing to DEFAULT stream.
     */
  virtual void dispatch(const shared_ptr<T> &data, const string &streamId = DEFAULT_STREAM_ID) {
    try {
      auto &streamReceivers = subscribers.get(streamId);
      for (auto &proc : streamReceivers) {
        proc->process(data);
      }
    } catch (KeyErrorException &e) {
      throw TopologyException("Dispatching error: Datahub[" + streamId + "] has not declared stream " + streamId);
    }
  }
};

template<typename T>
const string DataHub<T>::DEFAULT_STREAM_ID = "_DEFAULT_";

/*
 * Currently unused.
 */
template<typename S, typename R>
class IWKATS_CORE_EXPORT PeerHub : Component {
protected:
  shared_ptr<PeerHub<R, S>> peer;
public:
  PeerHub(const string &id) : Component(id) { }

  virtual ~PeerHub() { }

  virtual void connect(shared_ptr<PeerHub<R, S>> other) {
    peer = other;
    other->peer = this;
  }

  virtual void send(shared_ptr<S> data_ptr) {
    peer->receive(data_ptr);
  }

  virtual void receive(shared_ptr<R> data_ptr) = 0;
};

