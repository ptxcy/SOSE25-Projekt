#ifndef MESSAGE_PACK_HELPER_H
#define MESSAGE_PACK_HELPER_H

#include <msgpack.hpp>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>

/**
 * MessagePack-Hilfsfunktionen für die Serialisierung und Deserialisierung von Daten.
 * Verwendet die msgpack-c Bibliothek (https://github.com/msgpack/msgpack-c).
 */

namespace msgpack_helper {

/**
 * Serialisiert ein einzelnes Objekt zu MessagePack-Binärdaten
 * 
 * @tparam T Der Typ des zu serialisierenden Objekts
 * @param obj Das zu serialisierende Objekt
 * @return Vector mit den serialisierten Binärdaten
 */
template<typename T>
std::vector<uint8_t> pack(const T& obj) {
    std::stringstream buffer;
    msgpack::pack(buffer, obj);
    
    const std::string& str = buffer.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

/**
 * Deserialisiert MessagePack-Binärdaten zu einem Objekt
 * 
 * @tparam T Der Zieltyp der Deserialisierung
 * @param data Die zu deserialisierenden Binärdaten
 * @return Das deserialisierte Objekt
 * @throws msgpack::type_error bei Typfehlern oder Deserialisierungsfehlern
 */
template<typename T>
T unpack(const std::vector<uint8_t>& data) {
    try {
        msgpack::object_handle oh = msgpack::unpack(
            reinterpret_cast<const char*>(data.data()), 
            data.size()
        );
        
        msgpack::object obj = oh.get();
        T result;
        obj.convert(result);
        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "MessagePack deserialization error: " << e.what() << std::endl;
        throw;
    }
}

/**
 * Deserialisiert MessagePack-Binärdaten zu einem Objekt aus einem empfangenen String
 * Dies ist nützlich für die Verarbeitung von WebSocket-Nachrichten
 * 
 * @tparam T Der Zieltyp der Deserialisierung
 * @param data Der empfangene String mit Binärdaten
 * @param is_binary Flag, ob die Daten im Binärformat sind
 * @return Das deserialisierte Objekt
 * @throws msgpack::type_error bei Typfehlern oder Deserialisierungsfehlern
 */
template<typename T>
T unpack_from_message(const std::string& data, bool is_binary) {
    if (!is_binary) {
        throw std::runtime_error("Message is not in binary format, cannot unpack as MessagePack");
    }
    
    std::vector<uint8_t> binary_data(data.begin(), data.end());
    return unpack<T>(binary_data);
}

} // namespace msgpack_helper

#endif // MESSAGE_PACK_HELPER_H