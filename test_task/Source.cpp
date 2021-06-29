#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>


class folder {

public:

	int file_size;
	std::string file_name;
	std::string folder_name;

	std::vector<folder*> *subfolders = new std::vector<folder*>;
	folder() : file_size(0), file_name("None"), folder_name("None") {}
	folder(std::string folder, std::string file, int size) : folder_name(folder), file_name(file), file_size(size) {}
};

class tree {
public:
	std::vector<folder*> *root = new std::vector<folder *>;

	std::vector<folder*> **const_root = &root;

public:
	void insert(std::vector<std::string> path, std::string folder_name = "None", std::string file_name = "None", int size = 0);
	void print();
	void create_output_file();
	void sort();
private:
	void sort_bk();

	bool compare(const folder * lhs, const folder * rhs) {
		return lhs->folder_name < rhs->folder_name;
	}

private:
};

std::pair<std::string, int> file_info(std::string s) {
	int point_index = s.find_first_of('.');
	int space_index = s.find_first_of(' ');

	std::string name;
	int size;
	if ((point_index != -1 && space_index != -1) || (point_index == -1 && space_index != -1)) {
		name = s.substr(0, point_index);
		size = std::stoi(s.substr(space_index + 1));

		return std::make_pair(name, size);
	}

	return std::make_pair(s.substr(0, point_index), -1);
}


void tree::insert(std::vector<std::string> path, std::string folder_name, std::string file_name, int size) {

	auto is_exist_folder = [&folder_name](auto* i) { return ((i)->folder_name) == folder_name; };
	auto is_exist_file = [&file_name](auto* i) { return ((i)->file_name) == file_name; };

	for (int i = 0; i != path.size();) {
		for (auto c : path[i]) {
			if (isupper(c)) {
				folder_name = path[i];
				break;
			}
			else {
				std::pair<std::string, int> info = file_info((path[i]));
				folder_name = "None";
				file_name = info.first;
				size = info.second;
				break;
			}
		}
		auto tmp = std::find_if((*const_root)->begin(), (*const_root)->end(), is_exist_folder);
		if (tmp != (*const_root)->end() && ((*tmp)->folder_name == "None" && (*tmp)->file_name != "None")) {
			folder* t = new folder(folder_name, file_name, size);
			(*const_root)->push_back(t);
			auto d = (*const_root)->end() - 1;
			const_root = &(*d)->subfolders;
		}
		else if (tmp != (*const_root)->end()) {

			const_root = &(*tmp)->subfolders;
			std::vector<std::string> v;
			for (auto c = path.begin() + 1; c != path.end(); ++c) {
				v.push_back(*c);
			}
			insert(v);
			break;
		}
		else {
			folder* t = new folder(folder_name, file_name, size);
			(*const_root)->push_back(t);
			auto d = (*const_root)->end() - 1;
			const_root = &(*d)->subfolders;
		}
		++i;
	}

	const_root = &root;
}

std::vector<std::string> split_path(std::string path) {
	std::vector<std::string> fp;
	bool flag = true;
	size_t first_index = 0;
	size_t second_index = 0;
	while (flag) {
		second_index = path.find_first_of('\\', second_index);
		if (second_index != -1) {
			fp.push_back(path.substr(first_index, second_index - first_index));
			first_index = ++second_index;
		}
		else {
			fp.push_back(path.substr(first_index));
			flag = false;
		}
	}
	return fp;

}

void tree::sort_bk() { // работает не правильно. На одной из итераций выскакивает ошибка
	auto f = (*const_root)->begin();
	if ((*f)->folder_name != "None") {
		std::sort((*const_root)->begin(), (*const_root)->end(), [](const folder* lhs, const folder* rhs) {return lhs->folder_name < rhs->folder_name; });
	}
	if ((*f)->file_name != "None") {
		std::sort((*const_root)->begin(), (*const_root)->end(), [](const folder* lhs, const folder* rhs) {return lhs->file_name < rhs->file_name; });
		return;
	}
	for (int i = 0; i != (*const_root)->size(); ++i) {
		if ((*f)->file_name != "None") break;
		if ((*f)->subfolders->begin() == (*f)->subfolders->end()) break;
		auto d = (*const_root)[i].begin();
		const_root = &(*d)->subfolders;
		sort_bk();
	}
}

void tree::sort() {
	sort_bk();
}

void read_file(std::string file_name, tree* path_tree) {

	std::ifstream file(file_name);

	if (!file.is_open()) {
		std::cout << "The file was not opened\n";
		return;
	}
	std::string s;

	std::vector<std::vector<std::string>> pathes;

	while (std::getline(file, s)) {
		pathes.push_back(split_path(s));
	}

	for (int i = 0; i != pathes.size(); i++) {
		path_tree->insert(pathes[i]);
	}

		
}


int main() {

	tree *g = new tree();

	read_file("inp.txt", g);
	g->sort();

	return 0;
}