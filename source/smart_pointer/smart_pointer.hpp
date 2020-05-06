// Copyright 2020 Roger Peralta Aranibar
#ifndef SOURCE_SMART_POINTER_SMART_POINTER_HPP_
#define SOURCE_SMART_POINTER_SMART_POINTER_HPP_
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>
#include <set>

class Puntero_Iterador {
  public:
    Puntero_Iterador()
        : Iterador(0){};
    Puntero_Iterador(const Puntero_Iterador&) = delete;
    Puntero_Iterador& operator=(const Puntero_Iterador&) = delete;
    ~Puntero_Iterador(){}
    void reset() {Iterador = 0;}
    unsigned int get() {return Iterador;}
    void operator++() {Iterador++;}
    void operator++(int) {Iterador++;}
    void operator--() {Iterador--;}
    void operator--(int) {Iterador--;}
  private:
    unsigned int Iterador = 0;
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
  Puntero_Iterador* p_iterador;
 public:
  /* Constructor: SmartPointer(Type* resource=NULL)
   * Uso: SmartPointer<string> myPtr(new string);
   *      SmartPointer<string> myPtr;
   * ------------------------------------------------------------
   * Construye un nuevo objeto SmartPointer que administra el recurso
   * asignado. Se asume que el recurso viene de una llamada a new.
   * El recurso también podría ser NULL lo que ocasionaría que el
   * recurso no administre ningún recurso.
   */
  explicit SmartPointer(Type *resource = nullptr) :resource_(resource) 
  {
    resource_ = resource;
    p_iterador = new Puntero_Iterador();
    if (resource) 
    {
      (*p_iterador)++;
    }
  }
  /* Destructor: ~SmartPointer();
   * Uso: (implícito)
   * ------------------------------------------------------------
   * Decrementa el contador de referencias del recurso, eliminando
   * y liberando la memoria si fuera el último SmartPointer apuntando
   * al recurso.
   */
  ~SmartPointer() 
  {
    (*p_iterador)--;
      if (p_iterador->get() == 0) 
      {
        delete p_iterador;
        delete resource_;
      }
  }

  /* SmartPointer operadores de "des-referencia"(dereference)
   * Uso: cout << *myPtr << endl;
   * Uso: cout << myPtr->length << endl;
   * ------------------------------------------------------------
   * Permite al SmartPointer comportarse como si fuera un puntero.
   */
  Type &operator*()const{return *resource_;}
  Type *operator->()const{return resource_;}

  /* Funciones de copia
   * Uso: SmartPointer<string> ptr=existingPointer;
   *      ptr=existingPtr;
   * ------------------------------------------------------------
   * Cambia el recurso referenciado por este SmartPointer por otro
   * SmartPointer. Si el conteo llega a cero, debe ser eliminado
   * (deallocated).
   */
  void p_funtion_delete(){
    (*(p_iterador))--;
    if (p_iterador->get() == 0) 
    {
      delete p_iterador;
      delete resource_;
    }
  }
  SmartPointer &operator=(const SmartPointer &other) 
  {
    if ((resource_ == other.resource_) && 
        (p_iterador == other.p_iterador))
    {
      return *this;
    }
    p_funtion_delete();
    resource_ = other.resource_;
    p_iterador = other.p_iterador;
    if (resource_) 
    {
      (*p_iterador)++;
    }
    return *this;
  }
  SmartPointer &operator=(Type *other) 
  {
    p_funtion_delete();
    resource_ = other;
    p_iterador = new Puntero_Iterador();
    if (resource_) 
    {
      (*p_iterador)++;
    }
    return *this;
  }
  SmartPointer(const SmartPointer &other) 
  {
    resource_ = other.resource_;
    p_iterador = other.p_iterador;
    (*p_iterador)++;
  }
  /* Helper Function: Obtener recurso.
   * Uso: Type* p=GetPointer();
   * ------------------------------------------------------------
   * Retorna una variable puntero al recurso administrado.
   */
  Type *GetPointer() const {
    return resource_;  
  }
  /* Helper Function: Obtiene conteo
   * Uso: if (ptr.GetReferenceCount()==1) // Única referencia
   * ------------------------------------------------------------
   * Retorna el número de referencias apuntando al recurso.
   */
  size_t GetReferenceCount() const { 
    return p_iterador->get(); 
  }
  /* Helper Function: se des-asocia del recurso;
   * Uso: ptr.Detach();
   * ------------------------------------------------------------
   * Deja de administrar un recurso. eliminando y liberando la
   * memoria si es necesario.
   */
  void Detach()
  {

  }
};

#endif  // SOURCE_SMART_POINTER_SMART_POINTER_HPP_