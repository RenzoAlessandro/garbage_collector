// Copyright 2020 Roger Peralta Aranibar
#ifndef SOURCE_SMART_POINTER_SMART_POINTER_HPP_
#define SOURCE_SMART_POINTER_SMART_POINTER_HPP_
#include <cstddef>
#include <vector>
#include <set>

class Collectable {
  
public:
  Collectable() : gcRootCount(0), gcSequence(0) { }
  virtual ~Collectable() { }
  
private:
  friend class Collector;
  // conexiones según lo visto por el garbage collector
  std::vector<Collectable*> gcConnections;
  // Cuántas veces se hace referencia a este nodo como raíz
  int gcRootCount;
  // Número de secuencia utilizado para determinar si el objeto 
  // coleccionable ha sido visitado en la ronda actual de GC.
  size_t gcSequence;
};

class Collector {
public:
  static Collector& GetInstance();
  void AddRoot(Collectable*);
  void RemoveRoot(Collectable*);
  void AddEdge(Collectable*, Collectable*);
  void RemoveEdge(Collectable*, Collectable*);
  void ProcessEvents();
  void Collect();
  
private:
  
  Collector();

  struct Event {
    enum Type {
      AddRoot,
      RemoveRoot,
      Connect,
      Disconnect
    };
    Type type;
    Collectable* a;
    Collectable* b;
  };
};


/*
 * Smart Pointer que implementa un tipo de estrategia por conteo de referencias.
 * Permite que varios SmartPointers puedan acceder al mismo recurso compartido.
 * Cuando el ˙ltimo puntero es eliminado, se elimina y libera el recurso.
 */
template <typename Type>
class SmartPointer {
 private:
  Type *resource_;

 public:
 SmartPointer() : resource_(0) { }
  /* Constructor: SmartPointer(Type* resource=NULL)
   * Uso: SmartPointer<string> myPtr(new string);
   *      SmartPointer<string> myPtr;
   * ------------------------------------------------------------
   * Construye un nuevo objeto SmartPointer que administra el recurso
   * asignado. Se asume que el recurso viene de una llamada a new.
   * El recurso también podría ser NULL lo que ocasionaría que el
   * recurso no administre ningún recurso.
   */
  explicit SmartPointer(Type *resource) :resource_(resource) {
    _Retain();
  }

  /* Destructor: ~SmartPointer();
   * Uso: (implícito)
   * ------------------------------------------------------------
   * Decrementa el contador de referencias del recurso, eliminando
   * y liberando la memoria si fuera el último SmartPointer apuntando
   * al recurso.
   */
  ~SmartPointer() {
    _Retain();
  }


  /* SmartPointer operadores de "des-referencia"(dereference)
   * Uso: cout << *myPtr << endl;
   * Uso: cout << myPtr->length << endl;
   * ------------------------------------------------------------
   * Permite al SmartPointer comportarse como si fuera un puntero.
   */
  Type &operator*() const { return *resource_; }
  Type *operator->() const { assert(resource_); return resource_; }

  /* Funciones de copia
   * Uso: SmartPointer<string> ptr=existingPointer;
   *      ptr=existingPtr;
   * ------------------------------------------------------------
   * Cambia el recurso referenciado por este SmartPointer por otro
   * SmartPointer. Si el conteo llega a cero, debe ser eliminado
   * (deallocated).
   */
  SmartPointer &operator=(const SmartPointer &other) {
    if(resource_ != other.resource_) {
      _Release();
      resource_ = other.resource_;
      _Retain();
    }
    return *this;
  }
  SmartPointer &operator=(Type *other) {
    return *this;
  }
  SmartPointer(const SmartPointer &other) {
  }

  /* Helper Function: Obtener recurso.
   * Uso: Type* p=GetPointer();
   * ------------------------------------------------------------
   * Retorna una variable puntero al recurso administrado.
   */
  Type *GetPointer() const { return nullptr; }

  /* Helper Function: Obtiene conteo
   * Uso: if (ptr.GetReferenceCount()==1) // Única referencia
   * ------------------------------------------------------------
   * Retorna el número de referencias apuntando al recurso.
   */
  size_t GetReferenceCount() const { return 0; }

  /* Helper Function: se des-asocia del recurso;
   * Uso: ptr.Detach();
   * ------------------------------------------------------------
   * Deja de administrar un recurso. eliminando y liberando la
   * memoria si es necesario.
   */
  void Detach() {
  }
  void _Retain() {
    if(resource_) {
       Collector::GetInstance().AddRoot(resource_);
    }
  }
  
  void _Release() {
    if(resource_) {
       Collector::GetInstance().RemoveRoot(resource_);
    }
  }
  

};

template<class Type>
std::ostream& operator<<(std::ostream& out, const SmartPointer<Type>& p) {
  return out << p.resource_;
}

template<typename Type>
class EdgePtr {
  
public:

  EdgePtr(Collectable* owner) : _owner(owner), resource_(0) {
    assert(owner);
  }
  
  EdgePtr(Collectable* owner, const SmartPointer<Type>& other) : _owner(owner), resource_(other.resource_) {
    assert(owner);
    _Retain();
  }
  
  ~EdgePtr() {
  }
  
  EdgePtr& operator=(const EdgePtr& other) {
    assert(_owner == other._owner);
    if(resource_ != other.resource_) {
      _Release();
      resource_ = other.resource_;
      _Retain();
    }
    return *this;
  }
  
  template<class Type2>
  EdgePtr& operator=(const SmartPointer<Type2>& other) {
    if(resource_ != other.Get()) {
      _Release();
      resource_ = other.Get();
      _Retain();
    }
    return *this;
  }
  
  SmartPointer<Type> GetRootPtr() const { return SmartPointer<Type>(resource_); }
  
  operator bool() const { return resource_ != 0; }
  
  bool operator==(const EdgePtr& other) const {
    return resource_ == other.resource_;
  }
  
  bool operator!=(const EdgePtr& other) const {
    return resource_ != other.resource_;
  }
  
  bool operator<(const EdgePtr& other) const {
    return resource_ < other.resource_;
  }
  
private:
  
  void _Retain() {
    if(resource_) {
      Collector::GetInstance().AddEdge(_owner, resource_);
    }
  }
  
  void _Release() {
    if(resource_) {
      Collector::GetInstance().RemoveEdge(_owner, resource_);
    }
  }
  
  Collectable* _owner;
  Type* resource_;
  
}; // clase EdgePtr

#endif  // SOURCE_SMART_POINTER_SMART_POINTER_HPP_
