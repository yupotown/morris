#pragma once

#include <Siv3D.hpp>
#include "MorrisState.h"

struct Config {

	/// <summary>
	/// �t���X�N���[���ɂ��邩�ۂ�
	/// [Window.fullscreen]
	/// </summary>
	bool fullscreen;

	/// <summary>
	/// �E�B���h�E�T�C�Y�E�𑜓x
	/// [Window.size]
	/// </summary>
	Size resolution;

	/// <summary>
	/// �Ֆʂ̒��S�̈ʒu
	/// [Grid.center]
	/// </summary>
	Point gridCenter;

	/// <summary>
	/// �Ֆʂ�1�}�X�̑傫��
	/// [Grid.size]
	/// </summary>
	Size gridSize;

	/// <summary>
	/// �O���b�h���̐F
	/// [Grid.color]
	/// </summary>
	Color gridCol;

	/// <summary>
	/// �u���O�̃R�}��`�悷��ʒu
	/// [Piece.point1] - [Piece.point6]
	/// </summary>
	Point piecePos[2][3];

	/// <summary>
	/// �R�}�̔��a
	/// [Piece.radius]
	/// </summary>
	double pieceRad;

	/// <summary>
	/// �R�}�̐F
	/// [Piece.color1] [Piece.color2]
	/// </summary>
	Color pieceCol[2];

	/// <summary>
	/// �͂܂�Ă���R�}�̌��̏ꏊ�ɕ`�悷��F
	/// </summary>
	Color pieceColHold[2];

	/// <summary>
	/// �Ԃ⏟�������̃t�H���g
	/// [Turn.font] [Turn.fontsize]
	/// </summary>
	Font turnFont;

	/// <summary>
	/// �Ԃ⏟�������̈ʒu
	/// [Turn.point]
	/// </summary>
	Point turnPos;

	/// <summary>
	/// �Ԃ⏟�������̐F
	/// [Turn.color]
	/// </summary>
	Color turnCol;

	/// <summary>
	/// ���Z�b�g�{�^���̈ʒu
	/// [Reset.point]
	/// </summary>
	Point resetPos;

	/// <summary>
	/// ���Z�b�g�{�^���̃t�H���g
	/// [Reset.font]
	/// </summary>
	Font resetFont;

	Config() = default;

	/// <summary>
	/// �ݒ�� ini �t�@�C���ɏ������ށB
	/// </summary>
	void write(String file) const;

	/// <summary>
	/// �ݒ�� ini �t�@�C������ǂݍ��ށB
	/// �ǂݍ��݂Ɏ��s���������̓f�t�H���g�l�ƂȂ�B
	/// </summary>
	void read(String file);
	
	/// <summary>
	/// �O���b�h�̊e�_�̍��W�����߂�B
	/// </summary>
	Point gridPos(int x, int y) const;
};
