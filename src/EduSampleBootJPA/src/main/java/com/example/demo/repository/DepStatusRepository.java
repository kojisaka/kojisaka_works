package com.example.demo.repository;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.transaction.annotation.Transactional;

import com.example.demo.entity.DepStatusEntity;

public interface DepStatusRepository
	extends JpaRepository<DepStatusEntity, Integer> {

	public List<DepStatusEntity>
		findByNameAndType(String name, String type);

	@Transactional
	public void deleteByNameAndType(String name, String type);
}
