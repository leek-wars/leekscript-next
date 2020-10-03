// https://www.cs.uaf.edu/2007/fall/cs301/lecture/11_12_floating_asm.html
// https://www.cs.uaf.edu/courses/cs301/2014-fall/notes/float-asm/

struct T96 {
	int x;
	int y;
	int z;
};
struct D4 {
	double x;
	double y;
	double z;
	double w;
};

std::string fixed_array_to_string(Program* program, const Type* type, int* data) {
	Compiler& c = *program->compiler;
	llvm::StructType* llvm_type = (llvm::StructType*) type->llvm(c);
	size_t size = c.DL.getTypeAllocSize(llvm_type) / 4;
	auto layout = c.DL.getStructLayout(llvm_type);

	// std::cout << *((long*) data) << std::endl;
	// std::cout << *((long*) (data + 1)) << std::endl;
	// long v1 = *((long*) (data));
	// std::cout << "<< " << (v1 << 32) << std::endl;
	// std::cout << ">> " << (v1 >> 32) << std::endl;
	// long v2 = *((long*) (data + 1));
	// std::cout << "<< " << (v2 << 32) << std::endl;
	// std::cout << ">> " << (v2 >> 32) << std::endl;
	// long v3 = *((long*) (data + 2));
	// std::cout << "<< " << (v3 << 32) << std::endl;
	// std::cout << ">> " << (v3 >> 32) << std::endl;

	for (size_t i = 0; i < size + 5; ++i) {
		std::cout << data[i] << ", ";
	}
	std::cout << std::endl;

	// std::cout << "Fixed array type = " << type << " " << (void*) data << std::endl;

	std::string r = "[";
	for (size_t i = 0; i < type->size(); ++i) {
		if (i > 0) r += ", ";
		auto element_type = type->element(i);
		size_t raw_offset = layout->getElementOffset(i);
		size_t offset = raw_offset / 4;

		// std::cout << "element " << i << " offset = " << offset << " " << raw_offset << " : " << v << std::endl;

		if (element_type->is_bool()) {
			r += ((bool*) data)[raw_offset] ? "true" : "false";
		} else if (element_type->is_integer()) {
			r += std::to_string(data[offset]);
		} else if (element_type->is_long()) {
			long l = *((long*) (data + offset));
			r += std::to_string( l );
		} else if (element_type->is_real()) {
			double d = *((double*) (data + offset));
			long l = *((long*) (data + offset));
			std::cout << "d=" << d << " " << l << std::endl;
			r += std::to_string( d );
		} else if (element_type->is_fixed_array()) {
			r += fixed_array_to_string(program, element_type, data + offset);
		} else if (element_type->is_polymorphic()) {
			// int v2 = data[offset - 1];
			// int v3 = data[offset + 1];
			// std::cout << "v2 = " << v2 << " v = " << v << std::endl;
			// auto value = *((LSValue**) &v2);
			// auto value = (LSValue*) (((long) v2 << 16) + v);
			auto value = *((LSValue**) (data + offset));
			// std::cout << "value = " << (void*) value << std::endl;
			std::ostringstream oss;
			// oss << (void*) value;
			value->dump(oss, 5);
			assert(value->refs > 0);
			LSValue::delete_ref(value);
			r += oss.str();
		}
	}
	return r + "]";
}


if (type->is_fixed_array()) {
    llvm::StructType* llvm_type = (llvm::StructType*) type->llvm(*compiler);
    size_t size = compiler->DL.getTypeAllocSize(llvm_type) / 4;
    std::cout << "size = " << size << std::endl;

    if (type->size() <= 4) {
        // std::cout << "use 96 struct" << std::endl;

        auto layout = compiler->DL.getStructLayout(llvm_type);
        std::cout << "layout size = " << layout->getSizeInBits() << std::endl;
        for (int i = 0; i < type->size(); ++i) {
            std::cout << "element " << i << " " << layout->getElementOffset(i) << std::endl;
        }

        auto fun = (int (*)()) closure;

        auto v = fun();

        // if (size == 2 or size == 3) {
        // 	v.y = v.z;
        // 	asm("\t movl %%ecx,%0" : "=r"(v.z));
        // }

        double d;
        double d2;
        double d3;
        double d4;
        // asm("\t fstl (%0)" : "=r"(d3));
        asm("movd %%xmm0,%0" : "=r"(d));
        asm("movd %%xmm1,%0" : "=r"(d2));
        std::cout << d << " " << d2 << " " << d3 << " " << d4 << " " << std::endl;
        std::cout << "v " << v << std::endl;
        // std::cout << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;

        // return fixed_array_to_string(this, type, (int*) &v);
        return "";
    } else {
        // std::cout << type->llvm(*compiler) << std::endl;
        // auto layout = compiler->DL.getStructLayout(llvm_type);
        // for (size_t i = 0; i < type->size(); ++i) {
        // 	std::cout << "element " << i << " offset = " << layout->getElementOffset(i) << std::endl;
        // }
        // std::cout << "buffer size = " << size << std::endl;
        int v[size];
        auto fun = (void (*)(int*)) closure;
        fun(v);
        return fixed_array_to_string(this, type, v);
    }
}
