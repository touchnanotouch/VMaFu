// io/formats/_IFormat.hpp


#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

namespace vmafu {
    namespace io {

        // ============================================================================
        // КЛАСС: IFormat
        // НАЗНАЧЕНИЕ: Абстрактный интерфейс всех форматов файлов
        // ПРИНЦИП: Знает КАК читать/писать, но не знает ЧТО
        // ============================================================================

        class IFormat {
            public:
                // Деструктор

                virtual ~IFormat() = default;
                
                // ==================== ОСНОВНЫЕ ОПЕРАЦИИ ====================
                
                // Чтение всего файла
                virtual std::string read(const std::string& filename) const;
                
                // Чтение из потока (абстрактный - должны реализовать наследники)
                virtual std::string read_stream(std::istream& stream) const = 0;
                
                // Потоковое чтение больших файлов
                virtual std::string read_chunk(std::istream& stream, 
                                            size_t chunk_size = 4096) const;
                
                // Запись всего файла
                virtual void write(const std::string& filename, 
                                const std::string& data) const;
                
                // Запись в поток (абстрактный)
                virtual void write_stream(std::ostream& stream, 
                                        const std::string& data) const = 0;
                
                // Добавление в конец файла
                virtual void append(const std::string& filename, 
                                const std::string& data) const;
                
                // ==================== ИНФОРМАЦИЯ О ФОРМАТЕ ====================
                
                // Человеко-читаемое имя
                virtual std::string name() const = 0;
                
                // Расширение файла (.txt, .csv, .bin)
                virtual std::string extension() const = 0;
                
                // Бинарный ли формат
                virtual bool is_binary() const;
                
                // Поддерживает ли параллельный ввод-вывод
                virtual bool supports_parallel_io() const;
                
                // ==================== ВАЛИДАЦИЯ И ПРОВЕРКИ ====================
                
                // Проверка корректности файла
                virtual bool validate(const std::string& filename) const;
                
                // Оценка размера данных
                virtual size_t estimate_size(const std::string& data) const;
                
                // ==================== КОМПРЕССИЯ (опционально) ====================
                
                // Сжатие данных
                virtual std::string compress(const std::string& data) const;
                
                // Распаковка данных
                virtual std::string decompress(const std::string& data) const;
                
                // ==================== СТАТИЧЕСКИЕ УТИЛИТЫ ====================
                
                // Угадать формат по расширению
                static std::shared_ptr<IFormat> guess_format(const std::string& filename);
                
                // Проверить расширение файла
                static bool has_extension(const std::string& filename, 
                                        const std::string& ext);
            };

        using FormatPtr = std::shared_ptr<IFormat>;
    }
}


#include "detail/_IFormat.ipp"
