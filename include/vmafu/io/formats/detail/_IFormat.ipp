// io/formats/detail/_IFormat.ipp


namespace vmafu {
    namespace io {

        // ============================================================================
        // РЕАЛИЗАЦИИ МЕТОДОВ ПО УМОЛЧАНИЮ
        // ============================================================================

        // ----------------------------------------------------------------------------
        // read - чтение всего файла в память
        // ----------------------------------------------------------------------------
        std::string IFormat::read(const std::string& filename) const {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("IFormat: Cannot open file: " + filename);
            }
            
            return std::string(std::istreambuf_iterator<char>(file), 
                            std::istreambuf_iterator<char>());
        }

        // ----------------------------------------------------------------------------
        // read_chunk - потоковое чтение
        // ----------------------------------------------------------------------------
        std::string IFormat::read_chunk(std::istream& stream, 
                                    size_t chunk_size) const {
            std::string chunk(chunk_size, '\0');
            stream.read(&chunk[0], chunk_size);
            
            // Изменяем размер строки до фактически прочитанных байт
            chunk.resize(static_cast<size_t>(stream.gcount()));
            
            return chunk;
        }

        // ----------------------------------------------------------------------------
        // write - запись всего файла
        // ----------------------------------------------------------------------------
        void IFormat::write(const std::string& filename, 
                        const std::string& data) const {
            std::ofstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("IFormat: Cannot open file: " + filename);
            }
            
            file << data;
        }

        // ----------------------------------------------------------------------------
        // append - добавление в конец файла
        // ----------------------------------------------------------------------------
        void IFormat::append(const std::string& filename, 
                            const std::string& data) const {
            std::ofstream file(filename, std::ios::binary | std::ios::app);
            if (!file.is_open()) {
                throw std::runtime_error("IFormat: Cannot open file: " + filename);
            }
            
            file << data;
        }

        // ----------------------------------------------------------------------------
        // is_binary - по умолчанию текстовый формат
        // ----------------------------------------------------------------------------
        bool IFormat::is_binary() const {
            return false;
        }

        // ----------------------------------------------------------------------------
        // supports_parallel_io - по умолчанию не поддерживает
        // ----------------------------------------------------------------------------
        bool IFormat::supports_parallel_io() const {
            return false;
        }

        // ----------------------------------------------------------------------------
        // validate - проверка существования файла
        // ----------------------------------------------------------------------------
        bool IFormat::validate(const std::string& filename) const {
            std::ifstream file(filename);
            return file.good();
        }

        // ----------------------------------------------------------------------------
        // estimate_size - оценка размера (по умолчанию - размер строки)
        // ----------------------------------------------------------------------------
        size_t IFormat::estimate_size(const std::string& data) const {
            return data.size();
        }

        // ----------------------------------------------------------------------------
        // compress - по умолчанию без сжатия
        // ----------------------------------------------------------------------------
        std::string IFormat::compress(const std::string& data) const {
            return data;
        }

        // ----------------------------------------------------------------------------
        // decompress - по умолчанию без распаковки
        // ----------------------------------------------------------------------------
        std::string IFormat::decompress(const std::string& data) const {
            return data;
        }

        // ============================================================================
        // СТАТИЧЕСКИЕ МЕТОДЫ
        // ============================================================================

        // ----------------------------------------------------------------------------
        // has_extension - проверка расширения файла
        // ----------------------------------------------------------------------------
        bool IFormat::has_extension(const std::string& filename, 
                                const std::string& ext) {
            // Проверяем длину
            if (filename.length() < ext.length()) {
                return false;
            }
            
            // Сравниваем расширение
            return filename.compare(filename.length() - ext.length(), 
                                ext.length(), ext) == 0;
        }

        // ----------------------------------------------------------------------------
        // guess_format - угадать формат по расширению
        // ПРИМЕЧАНИЕ: Реализация будет позже, когда будут конкретные форматы
        // ----------------------------------------------------------------------------
        std::shared_ptr<IFormat> IFormat::guess_format(const std::string& filename) {
            // TODO: Реализовать после создания TextFormat, CSVFormat и т.д.
            // Пока возвращаем nullptr или базовый текстовый формат
            throw std::runtime_error("IFormat::guess_format not implemented yet");
            
            // Пример будущей реализации:
            // if (has_extension(filename, ".txt")) {
            //     return std::make_shared<TextFormat>();
            // } else if (has_extension(filename, ".csv")) {
            //     return std::make_shared<CSVFormat>();
            // }
            // throw std::runtime_error("Unknown file format: " + filename);
        }
    }
}
